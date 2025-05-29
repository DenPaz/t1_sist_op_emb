#ifndef CONFIG_H
#define CONFIG_H

#define ON 1
#define OFF 0

#define MAX_USER_TASKS 4
#define MAX_STACK_SIZE 32
#define MAX_PRIORITY 4

#define DEFAULT_SCHEDULER PRIORITY_SCHEDULER

#define IDLE_DEBUG OFF
#define DYNAMIC_MEM ON
#define PIPE_SIZE 4

#define _XTAL_FREQ 4000000UL

#endif /* CONFIG_H */
