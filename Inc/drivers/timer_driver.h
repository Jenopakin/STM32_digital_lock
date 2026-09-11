/*******************************************************************************
 * File Name    : timer_driver.h
 * Description  : Header for timer_driver.c - 1ms system tick using TIM2
 * Date         : 2026-09-06
 ******************************************************************************/
#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include "stm32f411xe.h"
#include <stdint.h>

void     TIMER_Init(void);
uint32_t TIMER_GetTick(void);
void     TIMER_DelayMs(uint32_t ms);

#endif /* TIMER_DRIVER_H */
