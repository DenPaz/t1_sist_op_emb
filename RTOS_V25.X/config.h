#ifndef CONFIG_H
#define CONFIG_H

#define ON 1
#define OFF 0

#define MAX_USER_TASKS 4
#define MAX_STACK_SIZE 32

#define DEFAULT_SCHEDULER PRIORITY_SCHEDULER

#define IDLE_DEBUG ON
#define DYNAMIC_MEM ON
#define PIPE_SIZE 3
#define _XTAL_FREQ 4000000

// IDs das tarefas
#define ID_ACEL 1
#define ID_CTRL 2
#define ID_INJ 3
#define ID_EST 4

// Prioridades (maior valor => maior prioridade)
#define PRIO_ACEL 1
#define PRIO_CTRL 2
#define PRIO_INJ 3
#define PRIO_EST 5

#endif /* CONFIG_H */
