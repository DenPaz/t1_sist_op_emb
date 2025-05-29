#ifndef IO_H
#define IO_H

#include <stdint.h>

// Configuração e leitura do ADC
void adc_init(void);
uint16_t adc_read(uint8_t channel);

// Configuração e controle do PWM (2 canais: CCP1 e CCP2)
void pwm_init(void);
void pwm_set_duty(uint8_t channel, uint16_t duty);

// Configuração da interrupção externa INT0 (one-shot)
void extint_init(void);

#endif /* IO_H */
