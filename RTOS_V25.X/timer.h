#ifndef TIMER_H
#define TIMER_H

void config_timer0(void);
void start_timer0(void);

// Tratador de interrupção do timer
void __interrupt() ISR_TMR0(void);

//interrup��o do bot�o pull-up ent�o ele gera a interrup��o em = 0

#endif /* TIMER_H */
