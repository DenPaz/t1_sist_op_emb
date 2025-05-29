#include "scheduler.h"
#include "config.h"
#include "types.h"

extern ready_queue_t r_queue;

// Guarda o último índice usado para cada prioridade
static uint8_t last_run_idx[MAX_PRIORITY+1];  

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
     uint8_t highest = 0xFF;
    // 1) encontra menor prioridade numérica entre READY
    for (uint8_t i = 1; i < r_queue.ready_queue_size; i++) {
        if (r_queue.ready_queue[i].task_state == READY &&
            r_queue.ready_queue[i].task_priority < highest) {
            highest = r_queue.ready_queue[i].task_priority;
        }
    }
    // 2) percorre em round-robin todas as tasks com essa prioridade
    uint8_t start = (last_run_idx[highest] + 1) % r_queue.ready_queue_size;
    for (uint8_t offset = 0; offset < r_queue.ready_queue_size; offset++) {
        uint8_t idx = (start + offset) % r_queue.ready_queue_size;
        tcb_t *t = &r_queue.ready_queue[idx];
        if (t->task_state == READY && t->task_priority == highest) {
            r_queue.task_running = idx;
            last_run_idx[highest] = idx;
            return;
        }
    }
    // 3) fallback para idle
    r_queue.task_running = 0;
}