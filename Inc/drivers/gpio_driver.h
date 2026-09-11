/*******************************************************************************
 * File Name    : gpio_driver.h
 * Description  : Header file for gpio_driver.c - generic GPIO register access
 * Date         : 2026-09-06
 ******************************************************************************/
#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f411xe.h"
#include <stdint.h>

/* Exported enum -------------------------------------------------------------*/
typedef enum
{
    GPIO_PULL_NONE = 0U,
    GPIO_PULL_UP   = 1U,
    GPIO_PULL_DOWN = 2U
} GPIO_PullType_t;

/* Exported define -----------------------------------------------------------*/
#define GPIO_PIN_STATE_LOW   (0U)
#define GPIO_PIN_STATE_HIGH  (1U)

/* Exported function prototypes ----------------------------------------------*/
void    GPIO_EnableClock(GPIO_TypeDef * const pGPIOx);
void    GPIO_ConfigOutputPP(GPIO_TypeDef * const pGPIOx, uint8_t pin);
void    GPIO_ConfigInput(GPIO_TypeDef * const pGPIOx, uint8_t pin, GPIO_PullType_t pull);
void    GPIO_SetPin(GPIO_TypeDef * const pGPIOx, uint8_t pin);
void    GPIO_ClearPin(GPIO_TypeDef * const pGPIOx, uint8_t pin);
void    GPIO_TogglePin(GPIO_TypeDef * const pGPIOx, uint8_t pin);
uint8_t GPIO_ReadPin(const GPIO_TypeDef * const pGPIOx, uint8_t pin);

#endif /* GPIO_DRIVER_H */
