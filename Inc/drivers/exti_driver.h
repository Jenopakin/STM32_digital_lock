/*******************************************************************************
 * File Name    : exti_driver.h
 * Description  : Header for exti_driver.c - PB4 button edge detection
 * Date         : 2026-09-06
 ******************************************************************************/
#ifndef EXTI_DRIVER_H
#define EXTI_DRIVER_H

#include "stm32f411xe.h"
#include <stdint.h>

/* Callback signature: driver calls this with the pin level AFTER the edge
 * (1 = released/idle, 0 = pressed), it never interprets duration/SHORT/LONG
 * itself - that logic belongs to the app layer (Week 2: code_decoder). */
typedef void (*EXTI_ButtonCallback_t)(uint8_t pinState);

void EXTI_Button_Init(void);
void EXTI_Button_RegisterCallback(EXTI_ButtonCallback_t callback);

#endif /* EXTI_DRIVER_H */
