#ifndef TIMER_H
#define TIMER_H

void config_timer0(void);
void start_timer0(void);

// Tratador de interrupção do timer
void __interrupt() interrupt_timer(void);

#endif /* TIMER_H */
