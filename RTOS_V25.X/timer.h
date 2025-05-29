#ifndef TIMER_H
#define TIMER_H

void config_timer0(void);
void start_timer0(void);

// Tratador de interrupÃ§Ã£o do timer
void __interrupt() ISR_TMR0(void);

//interrupção do botão pull-up então ele gera a interrupção em = 0

#endif /* TIMER_H */
