#include "io.h"
#include "config.h"
#include "kernel.h"
#include <xc.h>

void adc_init(void)
{
    TRISAbits.TRISA0 = 1;
    ADCON0 = 0b00000001;
    ADCON1 = 0b00001110;
    ADCON2 = 0b10101010;
}

uint16_t adc_read(uint8_t channel)
{
    ADCON0bits.CHS = channel;
    __delay_us(10);
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO)
        ;
    return (uint16_t)((ADRESH << 8) | ADRESL);
}

// Inicializa PWM nos canais CCP1 e CCP2
void pwm_init(uint8_t channel)
{
    if (channel == 1)
    {
        TRISCbits.TRISC2 = 0; // CCP1 como saída
        CCP1CON = 0b00001100; // Modo PWM
        T2CON = 0b00000100;   // Timer2 ligado, prescaler 1:1
        PR2 = 0xFF;           // Período
    }
}

// Ajusta duty cycle do PWM para o canal especificado
void pwm_set_duty(uint8_t channel, uint16_t duty)
{
    uint16_t max_duty = (uint16_t)((PR2 + 1) * 4);
    if (channel == 1)
    {
        if (duty >= max_duty)
        {
            duty = max_duty;
        }
        CCPR1L = (uint8_t)(duty >> 2);
        CCP1CONbits.DC1B = (uint8_t)(duty & 0b11);
    }
}

// Configura interrupção externa INT0 (rising edge)
void ext_int_init(uint8_t pin, uint8_t edge)
{
    if (pin == 0)
    {
        TRISBbits.TRISB0 = 1;
        INTCON2bits.INTEDG0 = edge;
        INTCONbits.INT0IE = 1;
        INTCONbits.INT0IF = 0;
    }
    INTCONbits.PEIE = 1; // Habilita interrupções periféricas
    INTCONbits.GIE = 1;  // Habilita interrupções globais
}
