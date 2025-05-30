#include <xc.h>
#include "user_app.h"
#include "kernel.h"
#include "sync.h"
#include "pipe.h"
#include "io.h"
#include "config.h"

TASK tarefa_acelerador(void)
{
    uint16_t pos;
    uint8_t acel;

    LATDbits.LATD2 = 0;

#define N_READINGS 3

    uint16_t readings[N_READINGS] = {0};
    uint8_t index = 0;
    uint32_t sum = 0;

    while (1)
    {
        sum -= readings[index];
        readings[index] = adc_read(0);
        sum += readings[index];
        index = (index + 1) % N_READINGS;
        pos = (uint16_t)(sum / N_READINGS);

        if (pos > 512)
        {
            LATDbits.LATD2 = 1; // LED ACEL ligado
        }
        else
        {
            LATDbits.LATD2 = 0; // LED ACEL desligado
        }
        acel = (uint8_t)(pos >> 2);
        write_pipe(&pipe_acel, acel);
        delay(25);
    }
}

TASK tarefa_controle_central(void)
{
    uint8_t pos;
    while (1)
    {
        read_pipe(&pipe_acel, &pos);
        mutex_lock(&mutex_injecao);
        tempo_injecao = pos * 4; // 0-255 -> 0-1020us
        mutex_unlock(&mutex_injecao);
        delay(10);
    }
}

TASK tarefa_injecao_eletronica(void)
{
    while (1)
    {
        mutex_lock(&mutex_injecao);
        uint16_t tempo;
        if (freio_ativo)
        {
            tempo = 0; // Freio ativo, não injeta
        }
        else
        {
            tempo = tempo_injecao; // Tempo de injeção calculado
        }
        mutex_unlock(&mutex_injecao);
        if (tempo > 255)
        {
            pwm_set_duty(1, 255); // Limita o duty cycle máximo
        }
        else
        {
            pwm_set_duty(1, tempo);
        }
        delay(5);
    }
}

TASK tarefa_controle_estabilidade(void)
{
    freio_ativo = 1; // Ativa o freio
    while (PORTBbits.RB0 == 0)
    {
        LATDbits.LATD1 = 1;
        LATDbits.LATD2 = ~LATDbits.LATD2;
        mutex_lock(&mutex_injecao);
        tempo_injecao = 0; // Desativa a injeção
        mutex_unlock(&mutex_injecao);
        delay(100); // Aguarda 100ms
    }

    LATDbits.LATD3 = 0;
    LATDbits.LATD4 = 0;
    freio_ativo = 0;
    tarefa_ce_ativa = 0;
    delete_task(tarefa_controle_estabilidade);

    while (1)
    {
        yield(); // Mantém a tarefa inativa
        LATDbits.LATD2 = 1;
    }
}

void user_interrupt(void)
{
    if (INTCONbits.INT0IF)
    {
        INTCONbits.INT0IF = 0;
        __delay_ms(20);
        if (PORTBbits.RB0 && !tarefa_ce_ativa)
        {
            freio_ativo = 1;
            tarefa_ce_ativa = 1;
            create_task(4, 4, tarefa_controle_estabilidade);
        }
    }
}

void user_config(void)
{
    create_pipe(&pipe_acel, 10);
    mutex_init(&mutex_injecao);

    adc_init();
    pwm_init(1);

    TRISDbits.RD2 = 0; // LED ACEL
    TRISDbits.RD3 = 0; // LED CTRL
    TRISDbits.RD4 = 0; // LED ESTABILIDADE
    TRISDbits.RD7 = 0; // LED IDLE (debug)
    LATD = 0x00;       // Todos LEDs desligados

    TRISBbits.TRISB0 = 1; // RB0 como entrada

    ext_int_init(0, 0);

    create_task(1, 3, tarefa_acelerador);
    create_task(2, 2, tarefa_controle_central);
    create_task(3, 1, tarefa_injecao_eletronica);

    asm("global _tarefa_acelerador, _tarefa_controle_central, _tarefa_injecao_eletronica, _tarefa_controle_estabilidade");
}
