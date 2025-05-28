#include "io.h"
#include "config.h"
#include <xc.h>

static void (*ext_int_callback)(void) = 0;

// ADC
void adc_init(void)
{
    ADCON0 = 0x00;       // Desabilita o ADC
    ADCON1 = 0x0E;       // AN0 analógico, outros digitais
    ADCON2 = 0b10101010; // A/D Clock = Fosc/32, 4 TAD, justificado à direita
}
uint16_t adc_read(uint8_t channel)
{
    ADCON0 = (channel << 2) | 1; // seleciona canal + liga ADC
    __delay_us(5);               // tempo de aquisição
    ADCON0bits.GO = 1;           // inicia conversão
    while (ADCON0bits.GO)
        ; // aguarda término
    return ((uint16_t)ADRESH << 8) | ADRESL;
}

// Inicialização do PWM (Timer2 + CCP1/CCP2)
void pwm_init(void)
{
    // Configura Timer2: prescaler 1:16, PR2 = 249 → período ~1 ms (Fosc=4 MHz)
    PR2 = 249;
    T2CON = 0b00000111; // TMR2ON=1, prescaler 1:16
    // CCP1 em modo PWM na saída RC2
    TRISCbits.TRISC2 = 0;
    CCP1CON = 0b00001100;
    // CCP2 em modo PWM na saída RC1
    TRISCbits.TRISC1 = 0;
    CCP2CON = 0b00001100;
}

// Ajusta duty cycle (0–100%) para o canal de PWM
void pwm_set(uint8_t channel, uint8_t duty_cycle)
{
    uint16_t duty = ((uint32_t)duty_cycle * (PR2 + 1)) / 100;
    switch (channel)
    {
    case 1:
        CCPR1L = duty >> 2;
        CCP1CONbits.DC1B = duty & 0x3;
        break;
    case 2:
        CCPR2L = duty >> 2;
        CCP2CONbits.DC2B1 = (duty >> 1) & 1;
        CCP2CONbits.DC2B0 = duty & 1;
        break;
    case 3:
        break;
    }
}

// Inicializa interrupção externa (INT0) e registra callback
void int_ext_init(void (*callback)(void))
{
    ext_int_callback = callback;
    // configure RB0/INT0 como entrada
    TRISBbits.TRISB0 = 1;
    // habilita weak pull-ups (opcional)
    INTCON2bits.RBPU = 0;
    // borda de subida
    INTCON2bits.INTEDG0 = 1;
    // limpa flag e habilita INT0
    INTCONbits.INT0IF = 0;
    INTCONbits.INT0IE = 1;
}
void ext_int_service(void)
{
    if (ext_int_callback)
        ext_int_callback();
}
