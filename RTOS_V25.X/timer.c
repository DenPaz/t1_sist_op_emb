#include <xc.h>
#include "timer.h"
#include "kernel.h"
#include "scheduler.h"
#include "user_app.h"

void config_timer0()
{
    // Habilita interrupções de periféricos
    INTCONbits.PEIE = 1;
    // Habilita interrupção do timer 0
    INTCONbits.TMR0IE = 1;
    // Seta o flag do timer em zero
    INTCONbits.TMR0IF = 0;
    // Transição do timer por referência interna
    T0CONbits.T0CS = 0;
    // Ativa prescaler para o timer zero
    T0CONbits.PSA = 0;
    // Prescaler 1:64
    T0CONbits.T0PS = 0b101;
    // Valor inicial do timer
    TMR0 = 0;
}

void start_timer0()
{
    T0CONbits.TMR0ON = 1;
}

extern volatile uint8_t freio_ativo;

// Tratador de interrupção do timer
void __interrupt() INTERRUPT_Handler()
{
    user_interrupt();
    if (INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF = 0;
        TMR0 = 0;
        decrease_time();
        SAVE_CONTEXT(READY);
        scheduler();
        RESTORE_CONTEXT();
    }
}
