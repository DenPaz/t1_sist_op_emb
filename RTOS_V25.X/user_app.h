#ifndef USER_APP_H
#define USER_APP_H

#include "types.h"
#include "pipe.h"
#include "sync.h"

TASK tarefa_acelerador(void);
TASK tarefa_controle_central(void);
TASK tarefa_injecao_eletronica(void);
TASK tarefa_controle_estabilidade(void);

void user_interrupt(void);
void user_config(void);

volatile uint8_t tarefa_ce_ativa = 0;
volatile uint8_t freio_ativo = 0;
pipe_t pipe_acel;
mutex_t mutex_injecao;
uint16_t tempo_injecao = 0;
uint16_t pwm_duty[2] = {0, 0}; // CCP1 (RC2) e CCP2 (RC1)

#endif /* USER_APP_H */
