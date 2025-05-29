#include <xc.h>
#include "timer.h"
#include "kernel.h"
#include "scheduler.h"
#include "user_app.h"   // para chamar tarefa_estabilidade

// Vetor único de interrupção (INT0 + TMR0)
void __interrupt() ISR(void)
{
    di();

    // Interrupção externa INT0 (botão de estabilidade)
    if (INTCONbits.INT0IF)
    {
        INTCONbits.INT0IF = 0;
        // Cria tarefa one-shot de estabilidade com máxima prioridade
        create_task(4, 1, tarefa_estabilidade);
        // Opcional: suspender outras tarefas para priorizar estabilidade
        for (uint8_t idx = 0; idx < r_queue.ready_queue_size; idx++) {
            uint8_t tid = r_queue.ready_queue[idx].task_id;
            if (tid != 0 && tid != 4) {
                r_queue.ready_queue[idx].task_state = WAITING;
            }
        }
    }

    // Timer0 para ticks do RTOS
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

// Configura Timer0 para gerar tick
void config_timer0()
{
    INTCONbits.PEIE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;

    T0CONbits.T0CS = 0;        // interno
    T0CONbits.PSA  = 0;        // usa prescaler
    T0CONbits.T0PS = 0b101;    // prescaler 1:64

    TMR0 = 0;
}

// Inicia Timer0
void start_timer0()
{
    T0CONbits.TMR0ON = 1;
}
