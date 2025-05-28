#include "kernel.h"
#include "user_app.h"

int main()
{
    os_init();

    create_task(1, 1, tarefa_acelerador);
    create_task(2, 2, tarefa_controle_central);
    create_task(3, 2, tarefa_injecao_eletronica);
    create_task(4, 3, tarefa_estabilidade);

    os_start();

    while (1)
        ;

    return 0;
}
