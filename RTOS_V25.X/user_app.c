#include <xc.h>
#include "user_app.h"
#include "kernel.h"
#include "sync.h"
#include "pipe.h"
#include "io.h"
#include "config.h"

// Recursos compartilhados
pipe_t pipe;
mutex_t m;
uint16_t shared_buffer;

TASK tarefa_acelerador(void)
{
    uint16_t pos;
    while (1)
    {
        pos = adc_read(0);                        // lê pedal
        write_pipe(&pipe, (uint8_t)(pos >> 8));   // envia MSB
        write_pipe(&pipe, (uint8_t)(pos & 0xFF)); // envia LSB
        LATDbits.LATD0 = !LATDbits.LATD0;         // pisca LED ACEL
        delay(10);
    }
}

TASK tarefa_controle(void)
{
    uint8_t hi, lo;
    uint16_t pos, duty, max;
    while (1)
    {
        read_pipe(&pipe, &hi);
        read_pipe(&pipe, &lo);
        pos = ((uint16_t)hi << 8) | lo;

        max = ((uint16_t)PR2 + 1) * 4 - 1;
        duty = (pos * max) / 1023; // mapeia 0-1023 → duty

        mutex_lock(&m);
        shared_buffer = duty;
        mutex_unlock(&m);

        LATDbits.LATD1 = !LATDbits.LATD1; // pisca LED CTRL
    }
}

TASK tarefa_injecao(void)
{
    uint16_t duty;
    while (1)
    {
        mutex_lock(&m);
        duty = shared_buffer;
        mutex_unlock(&m);

        pwm_set_duty(1, duty); // CCP1 (RC2)
        pwm_set_duty(2, duty); // CCP2 (RC1)

        delay(1); // aguarda PWM estabilizar
    }
}

TASK tarefa_estabilidade(void)
{
    LATDbits.LATD2 = 1; // LED ESTABILIDADE ligado
    while (1)
        ; // permanece ativa (one-shot)
}

void user_config(void)
{
    TRISAbits.TRISA0 = 1; // AN0 (pedal) como entrada
    TRISDbits.TRISD0 = 0; // LED ACEL = saída
    TRISDbits.TRISD1 = 0; // LED CTRL = saída
    TRISDbits.TRISD2 = 0; // LED ESTABILIDADE = saída
    TRISDbits.TRISD7 = 0; // LED IDLE (debug)

    adc_init();
    pwm_init();
    extint_init();

    mutex_init(&m);
    create_pipe(&pipe, PIPE_SIZE);

    create_task(ID_ACEL, PRIO_ACEL, tarefa_acelerador);
    create_task(ID_CTRL, PRIO_CTRL, tarefa_controle);
    create_task(ID_INJ, PRIO_INJ, tarefa_injecao);

    asm("global _tarefa_acelerador, _tarefa_controle, _tarefa_injecao, _tarefa_estabilidade");
}
