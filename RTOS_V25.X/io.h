#ifndef IO_H
#define IO_H

#include <stdint.h>

// ADC
void adc_init(void);
uint16_t adc_read(uint8_t channel);

// PWM
void pwm_init(void);
void pwm_set(uint8_t channel, uint8_t duty_cycle);

// Interrupção externa
void int_ext_init(void (*callback)(void));
void ext_int_service(void);

#endif /* IO_H */
