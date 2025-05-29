#include "sync.h"
#include "types.h"
#include "kernel.h"
#include "scheduler.h"
#include <xc.h>

// Declara fila de aptos
extern ready_queue_t r_queue;

// API
void sem_init(sem_t *sem, int16_t value)
{
    sem->s_value = value;
    sem->s_size = 0;
    sem->s_pos_out = 0;
}

void sem_wait(sem_t *sem)
{
    di();
    sem->s_value--;
    if (sem->s_value < 0)
    {
        // Bloqueia a tarefa
        sem->s_queue[sem->s_size] = r_queue.task_running;
        sem->s_size = (sem->s_size + 1) % MAX_USER_TASKS;
        // Força a preempção
        SAVE_CONTEXT(SEM_WAITING);
        scheduler();
        RESTORE_CONTEXT();
    }
    ei();
}

void sem_post(sem_t *sem)
{
    di();
    sem->s_value++;
    if (sem->s_value <= 0)
    {
        // Desbloqueia tarefa mais antiga
        r_queue.ready_queue[sem->s_queue[sem->s_pos_out]].task_state = READY;
        sem->s_pos_out = (sem->s_pos_out + 1) % MAX_USER_TASKS;
    }
    ei();
}

// API para o mutex
void mutex_init(mutex_t *m)
{
    m->flag = true; // Seção crítica livre
    m->s_size = 0;
    m->s_pos_out = 0;
}

void mutex_lock(mutex_t *m)
{
    di();
    if (m->flag)
    {
        // Adquire o mutex
        m->flag = false;
        ei();
    }
    else
    {
        // Bloqueia a tarefa atual
        m->s_queue[m->s_size] = r_queue.task_running;
        m->s_size = (m->s_size + 1) % MAX_USER_TASKS;
        SAVE_CONTEXT(SEM_WAITING);
        scheduler();
        RESTORE_CONTEXT();
        ei();
    }
}

void mutex_unlock(mutex_t *m)
{
    di();
    if (m->s_pos_out != m->s_size)
    {
        // Desbloqueia a próxima tarefa na fila
        uint8_t next = m->s_queue[m->s_pos_out];
        r_queue.ready_queue[next].task_state = READY;
        m->s_pos_out = (m->s_pos_out + 1) % MAX_USER_TASKS;
    }
    else
    {
        // Nenhuma tarefa bloqueada, libera o mutex
        m->flag = true;
    }
    ei();
}
