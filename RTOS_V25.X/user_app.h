#ifndef USER_APP_H
#define USER_APP_H

#include "types.h"

TASK tarefa_acelerador(void);
TASK tarefa_controle(void);
TASK tarefa_injecao(void);
TASK tarefa_estabilidade(void);

void user_config(void);

#endif /* USER_APP_H */
