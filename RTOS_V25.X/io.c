#include "io.h"
#include "config.h"
#include <xc.h>

// Inicializa o ADC (AN0 analógico, FOSC/32, justificado à direita)
void adc_init(void)
{
    ADCON1 = 0b00001110; // AN0 analógico, demais digital
    ADCON2 = 0b10101010; // Fosc/32, 12 TAD
    ADCON0bits.ADON = 1;
}

// Lê canal ADC (0–13), retorna 10 bits
uint16_t adc_read(uint8_t channel)
{
    ADCON0 &= 0b11000111;
    ADCON0 |= (channel << 3) & 0b00111000;
    __delay_us(5);
    ADCON0bits.GO_nDONE = 1;
    while (ADCON0bits.GO_nDONE)
        ;
    return ((uint16_t)ADRESH << 8) | ADRESL;
}

// Inicializa PWM nos canais CCP1 e CCP2
void pwm_init(void)
{
    TRISCbits.TRISC2 = 0; // CCP1 como saída
    TRISCbits.TRISC1 = 0; // CCP2 como saída

    PR2 = 249;               // Período
    T2CONbits.T2CKPS = 0b01; // Prescaler 1:4
    T2CONbits.TMR2ON = 1;    // Liga Timer2

    CCP1CON = 0b00001100; // Modo PWM
    CCP2CON = 0b00001100; // Modo PWM
}

// Ajusta duty (0..(PR2+1)*4-1) em cada canal
void pwm_set_duty(uint8_t channel, uint16_t duty)
{
    uint16_t max = ((uint16_t)PR2 + 1) * 4 - 1;
    if (duty > max)
        duty = max;

    switch (channel)
    {
    case 1:
        CCPR1L = (uint8_t)(duty >> 2);
        CCP1CON = (uint8_t)((CCP1CON & 0xCF) | ((duty & 0x03) << 4));
        break;
    case 2:
        CCPR2L = (uint8_t)(duty >> 2);
        CCP2CON = (uint8_t)((CCP2CON & 0xCF) | ((duty & 0x03) << 4));
        break;
    default:
        break;
    }
}

// Configura interrupção externa INT0 (rising edge)
void extint_init(void)
{
    TRISBbits.TRISB0 = 1;    // RB0/INT0 como entrada
    INTCON2bits.INTEDG0 = 1; // borda de subida
    INTCONbits.INT0IF = 0;   // limpa flag
    INTCONbits.INT0IE = 1;   // habilita INT0
}
