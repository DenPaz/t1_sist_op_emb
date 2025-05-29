#include "io.h"
#include "config.h"
#include "kernel.h"
#include <xc.h>

static void (*ext_int_callback)(void) = 0;

// ADC
void adc_init(void)
{
    ADCON0 = 0x01;       // Habilita o ADC
    ADCON1 = 0x0E;       // AN0 analÃ³gico, outros digitais
    ADCON2 = 0b10101010; // A/D Clock = Fosc/32, 4 TAD, justificado Ã  direita
    TRISAbits.RA0 = 1;
}

uint16_t adc_read(void)
{
    delay(5);
    ADCON0bits.GO = 1;          // Inicia conversão
    while (ADCON0bits.GO);      // Aguarda fim da conversão
    return ((uint16_t)ADRESH << 8) | ADRESL;
}

// === PWM (CCP1 / RC2) ===
void pwm_init(void)
{
    TRISCbits.RC2 = 0;          // RC2 como saída (CCP1)

    T2CON = 0b00000111;         // Timer2 ligado, prescaler 1:16
    PR2 = 255;                  // Período PWM

    CCP1CON = 0b00001100;       // Modo PWM
    CCPR1L = 0;
    CCP1CONbits.DC1B = 0;
}

void pwm_set(uint8_t duty_cycle)
{
    uint16_t duty = duty_cycle;
    duty = duty << 2;           // Expande 8 bits (0?255) para 10 bits

    if (duty > 1023) duty = 1023;

    CCPR1L = duty >> 2;             // 8 bits mais significativos
    CCP1CONbits.DC1B = duty & 0x03; // 2 bits menos significativos
}

// === INT0 ===
void int_ext_init(void)
{
    TRISBbits.RB0 = 1;          // RB0 como entrada
    INTCON2bits.INTEDG0 = 1;    // Borda de subida
    INTCONbits.INT0IF = 0;      // Limpa flag
    INTCONbits.INT0IE = 1;      // Habilita INT0
    INTCONbits.GIE = 1;         // Habilita interrupções globais
}