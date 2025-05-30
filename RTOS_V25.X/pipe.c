#include "pipe.h"
#include "mem.h"

void create_pipe(pipe_t *p, uint8_t size)
{
    p->pipe_msg = (uint8_t *)SRAMalloc(size);
    p->pipe_size = size;
    p->pipe_pos_read = 0;
    p->pipe_pos_write = 0;
    mutex_init(&p->pipe_mutex);
    sem_init(&p->pipe_sem_read, 0);
    sem_init(&p->pipe_sem_write, size);
}

void delete_pipe(pipe_t *p)
{
    SRAMfree(p->pipe_msg);
    p->pipe_msg = NULL;
}

void write_pipe(pipe_t *p, uint8_t data)
{
    di();
    mutex_lock(&p->pipe_mutex);
    sem_wait(&p->pipe_sem_write);
    p->pipe_msg[p->pipe_pos_write] = data;
    p->pipe_pos_write = (p->pipe_pos_write + 1) % PIPE_SIZE;
    sem_post(&p->pipe_sem_read);
    mutex_unlock(&p->pipe_mutex);
    ei();
}

void read_pipe(pipe_t *p, uint8_t *data)
{
    di();
    sem_wait(&p->pipe_sem_read);
    *data = p->pipe_msg[p->pipe_pos_read];
    p->pipe_pos_read = (p->pipe_pos_read + 1) % PIPE_SIZE;
    sem_post(&p->pipe_sem_write);
    ei();
}
