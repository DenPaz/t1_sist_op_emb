#include "kernel.h"
#include "user_app.h"

int main()
{
    os_init();

    create_task(1, 4, tarefa_acelerador);
    create_task(2, 3, tarefa_controle_central);
    create_task(3, 3, tarefa_injecao_eletronica);
    // create_task(4, 1, tarefa_estabilidade); // Tomar cuidado para tarefa não ir para o escalonador -> criar ao clicar no botão

    os_start();

    while (1)
        ;

    return 0;
}
