#include "io.h"
#include "config.h"
#include "kernel.h"
#include <xc.h>

void adc_init(void)
{
    ADCON0 = 0x01;
    ADCON1 = 0x0E;
    ADCON2 = 0b10101010;
    TRISAbits.RA0 = 1;
}

uint16_t adc_read(uint8_t channel)
{
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO)
        ;
    return ((uint16_t)ADRESH << 8) | ADRESL;
}

// Inicializa PWM nos canais CCP1 e CCP2
void pwm_init(void)
{
    TRISCbits.RC2 = 0; // CCP1 como saída
    TRISCbits.RC1 = 0; // CCP2 como saída

    PR2 = 255;               // Período
    T2CONbits.T2CKPS = 0b01; // Prescaler 1:4
    T2CONbits.TMR2ON = 1;    // Liga Timer2

    CCP1CON = 0b00001100; // Modo PWM
    CCPR1L = 0;
    CCP1CONbits.DC1B = 0;
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
    TRISBbits.TRISB0 = 1;
    INTCON2bits.INTEDG0 = 1;
    INTCONbits.INT0IF = 0;
    INTCONbits.INT0IE = 1;
}
