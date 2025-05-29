#include "scheduler.h"
#include "config.h"
#include "types.h"

extern ready_queue_t r_queue;

void __reentrant scheduler()
{
#if DEFAULT_SCHEDULER == RR_SCHEDULER
    rr_scheduler();
#elif DEFAULT_SCHEDULER == PRIORITY_SCHEDULER
    priority_scheduler();
#endif
}

void __reentrant rr_scheduler()
{
    uint8_t idle_task = 0;
    do
    {
        r_queue.task_running = (r_queue.task_running + 1) % r_queue.ready_queue_size;
        if (r_queue.task_running == 0)
        {
            idle_task++;
            if (idle_task == 2)
                break;
        }
    } while (r_queue.ready_queue[r_queue.task_running].task_state != READY ||
             r_queue.task_running == 0);
}

void __reentrant priority_scheduler()
{
    // Seleciona tarefa READY com menor valor de priority (0 = maior prioridade)
    uint8_t best_priority = 0xFF;
    int selected = -1;
    for (uint8_t i = 0; i < r_queue.ready_queue_size; i++) {
        tcb_t *t = &r_queue.ready_queue[i];
        if (t->task_state == READY) {
            if ((uint8_t)t->task_priority < best_priority) {
                best_priority = t->task_priority;
                selected = i;
            }
        }
    }
    // Se não encontrou READY, seleciona idle (índice 0)
    r_queue.task_running = (selected >= 0) ? selected : 0;
}