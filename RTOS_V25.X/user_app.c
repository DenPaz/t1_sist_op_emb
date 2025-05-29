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
        uint16_t raw = adc_read();
        uint8_t data = (uint8_t)(raw >> 2); // reduz para 8 bits
        write_pipe(&accel_pipe, data);
        delay(10); // 10 ticks (ajustar conforme necessidade)
    }
}

// Tarefa 2: recebe do PIPE, calcula duty e grava no buffer
TASK tarefa_controle_central()
{
    while (1)
    {
        uint8_t data = 0;
        read_pipe(&accel_pipe, &data);
        uint8_t duty = data % 100;       // exemplo de mapeamento

        mutex_lock(&buffer_mutex);
        accel_data = duty;
        mutex_unlock(&buffer_mutex);

        delay(5);
    }
}

// Tarefa 3: lê buffer e ajusta PWM (CCP1)
TASK tarefa_injecao_eletronica()
{
    while (1)
    {
        uint8_t duty;
        mutex_lock(&buffer_mutex);
        duty = accel_data;
        mutex_unlock(&buffer_mutex);

        pwm_set(duty);   // usa apenas CCP1
        delay(5);
    }
}

// Tarefa 4: controle de estabilidade one-shot
TASK tarefa_estabilidade()
{
    while (1)
    {
        if (est_flag)
        {
            est_flag = 0;
            LATDbits.LD3 = 1; // aciona freios
            delay(100);
            LATDbits.LD3 = 0;
            
        }
        // bloqueia atÃ© prÃ³xima interrupÃ§Ã£o
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
