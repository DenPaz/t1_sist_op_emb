#include "user_app.h"
#include "kernel.h"
#include "io.h"
#include "sync.h"
#include "pipe.h"
#include "config.h"
#include <xc.h>

// Recursos compartilhados
pipe_t accel_pipe;
mutex_t buffer_mutex;
volatile uint8_t accel_data = 0;
volatile uint8_t est_flag = 0;

// Tarefa 1: lê ADC e envia via PIPE
TASK tarefa_acelerador()
{
    while (1)
    {
        uint16_t raw = adc_read(0);
        uint8_t data = (uint8_t)(raw >> 2); // reduz para 8 bits
        write_pipe(&accel_pipe, data);
        delay(10); // 10 ticks (ajustar conforme necessidade)
    }
}

// Tarefa 2: recebe do PIPE, calcula tempo de injeção e escreve no buffer com mutex
TASK tarefa_controle_central()
{
    while (1)
    {
        uint8_t data = 0;
        read_pipe(&accel_pipe, &data);
        // mapa simples: duty_cycle = data % 100
        uint8_t duty = data % 100;

        mutex_lock(&buffer_mutex);
        accel_data = duty;
        mutex_unlock(&buffer_mutex);

        delay(5);
    }
}

// Tarefa 3: lê buffer e ajusta PWM nos bicos
TASK tarefa_injecao_eletronica()
{
    while (1)
    {
        uint8_t duty;
        mutex_lock(&buffer_mutex);
        duty = accel_data;
        mutex_unlock(&buffer_mutex);

        pwm_set(1, duty);
        pwm_set(2, duty);
        // pwm_set(3, duty); // canal software

        delay(5);
    }
}

// Callback da interrupção externa
static void stability_callback(void)
{
    est_flag = 1;
}

// Tarefa 4: controle de estabilidade one-shot
TASK tarefa_estabilidade()
{
    while (1)
    {
        if (est_flag)
        {
            LATDbits.LD3 = 1; // aciona freios
            delay(100);
            LATDbits.LD3 = 0;
            est_flag = 0;
        }
        // bloqueia até próxima interrupção
        change_state(WAITING);
    }
}

// Configuração de periféricos e recursos
void user_config()
{
    // LEDs e botões
    TRISDbits.RD0 = 0; // injeção 3
    TRISDbits.RD1 = 0; // acelerador
    TRISDbits.RD2 = 0; // controle central
    TRISDbits.RD3 = 0; // estabilidade

    create_pipe(&accel_pipe);
    mutex_init(&buffer_mutex);

    // Init IO
    adc_init();
    pwm_init();
    int_ext_init(stability_callback);

    // Define globais para otimização
    asm("global _tarefa_acelerador, _tarefa_controle_central, _tarefa_injecao_eletronica, _tarefa_estabilidade");
}
