#ifndef IO_H
#define IO_H

#include <stdint.h>

// Configuração e leitura do ADC
void adc_init(void);
uint16_t adc_read(uint8_t channel);

// Configuração e controle do PWM (2 canais: CCP1 e CCP2)
void pwm_init(uint8_t channel);
void pwm_set_duty(uint8_t channel, uint16_t duty);

// Configuração da interrupção externa
void ext_int_init(uint8_t pin, uint8_t edge);

#endif /* IO_H */
