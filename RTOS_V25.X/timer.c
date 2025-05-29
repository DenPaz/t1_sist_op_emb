#include <xc.h>
#include "timer.h"
#include "kernel.h"
#include "scheduler.h"
#include "user_app.h"   // para chamar tarefa_estabilidade
#include <stdbool.h>
extern ready_queue_t r_queue;

// Vetor único de interrupção (INT0 + TMR0)
static bool est_created = false;

void __interrupt() ISR(void)
{
    di();

    // Interrupção externa INT0 (botão de estabilidade)
    if (INTCONbits.INT0IF)
    {
        INTCONbits.INT0IF = 0;

        if (!est_created) {
            // primeira vez: cria a tarefa
            create_task(4, 1, tarefa_estabilidade);
            est_created = true;
        } else {
            // subsequentes: apenas reabilita a tarefa já existente
            for (uint8_t i = 0; i < r_queue.ready_queue_size; i++) {
                if (r_queue.ready_queue[i].task_id == 4) {
                    r_queue.ready_queue[i].task_state = READY;
                    break;
                }
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
