#include <xc.h>
#include "timer.h"
#include "kernel.h"
#include "scheduler.h"
#include "io.h"

// Vetor único de interrupção (INT0 + TMR0)
void __interrupt() ISR(void)
{
    di();

    // Interrupção externa INT0
    if (INTCONbits.INT0IF)
    {
        INTCONbits.INT0IF = 0;
        ext_int_service();
    }

    // Timer0
    if (INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF = 0;
        TMR0 = 0;

        decrease_time();

        SAVE_CONTEXT(READY);
        scheduler();
        RESTORE_CONTEXT();
    }

    ei();
}

void config_timer0()
{
    INTCONbits.PEIE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS = 0b101; // prescaler 1:64
    TMR0 = 0;
}

void start_timer0()
{
    T0CONbits.TMR0ON = 1;
}