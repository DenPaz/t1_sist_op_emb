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

// Conferir este escalonador -> e se tiverem duas tasks com mesma prioridade?
void __reentrant priority_scheduler()
{
    uint8_t highest_priority = 0;
    uint8_t selected = 0;

    for (uint8_t i = 1; i < r_queue.ready_queue_size; i++)
    {
        if (r_queue.ready_queue[i].task_state == READY &&
            r_queue.ready_queue[i].task_priority > highest_priority)
        {
            highest_priority = r_queue.ready_queue[i].task_priority;
            selected = i;
        }
    }
    r_queue.task_running = selected;
}
