#include "user_app.h"
#include "kernel.h"
#include "io.h"
#include "sync.h"
#include "pipe.h"
#include "config.h"
#include "timer.h"
#include <xc.h>

// Recursos compartilhados
pipe_t accel_pipe;
mutex_t buffer_mutex;
volatile uint8_t accel_data = 0;
volatile uint8_t est_flag = 0;

// Tarefa 1: lÃª ADC e envia via PIPE
TASK tarefa_acelerador()
{
    while (1)
    {
        LATDbits.LATD1 = 1;                // LED T2 ON
        uint16_t raw = adc_read();
        uint8_t data = raw >> 2;
        write_pipe(&accel_pipe, data);
        LATDbits.LATD1 = 0;                // LED T2 OFF
        yield();
    }
}

// Tarefa 2: recebe do PIPE, calcula duty e grava no buffer
TASK tarefa_controle_central()
{
    while (1)
    {
        LATDbits.LATD2 = 1;  // LED T3 ON
        uint8_t data;
        read_pipe(&accel_pipe, &data);
        uint8_t duty = data % 100;
        mutex_lock(&buffer_mutex);
        accel_data = duty;
        mutex_unlock(&buffer_mutex);
        LATDbits.LATD2 = 0;  // LED T3 OFF
        yield();
    }
}

TASK tarefa_injecao_eletronica()
{
    while (1)
    {
        LATDbits.LATD0 = 1;  // LED T1 ON
        static uint8_t last_duty = 255;
        uint8_t duty;

        mutex_lock(&buffer_mutex);
        duty = accel_data;
        mutex_unlock(&buffer_mutex);

        if (duty != last_duty)
        {
            pwm_set(duty);
            last_duty = duty;
        }

        yield();
        LATDbits.LATD0 = 0;  // LED T1 OFF
        yield();
    }
}

TASK tarefa_estabilidade()
{
    while (1)
    {
        if (est_flag)
        {
            est_flag = 0;
            LATDbits.LATD3 = 1;  // LED T4 ON
            delay(2);
            LATDbits.LATD3 = 0;  // LED T4 OFF
        }
        change_state(WAITING);
    }
}


void user_config()
{
    // Configura pinos de indicadores (opcional)
    TRISDbits.RD0 = 0; // T1 (injeção)
    TRISDbits.RD1 = 0; // T2 (acelerador)
    TRISDbits.RD2 = 0; // T3 (controle central)
    TRISDbits.RD3 = 0; // T4 (estabilidade)
    LATD = 0;          // Zera todos os LEDs

    // Inicializa comunicação entre tarefas
    create_pipe(&accel_pipe);
    mutex_init(&buffer_mutex);

    // Inicializa periféricos
    adc_init();        // ADC em AN0
    pwm_init();        // PWM em CCP1 (RC2)
    int_ext_init();    // Configura INT0 para tarefa de estabilidade

    // Configura Timer0 para ticks do RTOS
    config_timer0();
    start_timer0();

    // Marcações para o linker
    asm("global _tarefa_acelerador, _tarefa_controle_central, _tarefa_injecao_eletronica, _tarefa_estabilidade");
}
