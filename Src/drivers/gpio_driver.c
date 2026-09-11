/*******************************************************************************
 * File Name    : gpio_driver.c
 * Description  : Generic GPIO register-level driver (no HAL)
 * Date         : 2026-09-06
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "gpio_driver.h"

/* Private define -------------------------------------------------------------
 * MODER / PUPDR use 2 bits per pin -> mask 0x3
 * ---------------------------------------------------------------------------*/
#define GPIO_MODER_MASK   (0x3UL)
#define GPIO_PUPDR_MASK   (0x3UL)
#define GPIO_MODER_OUTPUT (0x1UL)
#define GPIO_MODER_INPUT  (0x0UL)

/* Public functions ----------------------------------------------------------*/

/*********************************************************************
 * @fn      		  - GPIO_EnableClock
 * @brief             - Enable AHB1 clock for the given GPIO port
 * @param[in]         - pGPIOx : base address of GPIO port (GPIOA, GPIOB, ...)
 * @return            - none
 * @Note              - Must be called before any other GPIO_* function on
 *                       the same port; RCC clock enable requires a bit-set
 *                       only (no clear needed since other bits are unrelated
 *                       ports).
 *////////////////////////////////////////////////////////////////////
void GPIO_EnableClock(GPIO_TypeDef * const pGPIOx)
{
    if (pGPIOx == GPIOA)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    }
    else if (pGPIOx == GPIOB)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    }
    else if (pGPIOx == GPIOC)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    }
    else
    {
        /* MISRA: default branch required, no other port used in this project */
    }

    /* Dummy read-back: workaround for clock-enable timing issue across
     * different compiler optimization levels (see Exam 4 debugging notes) */
    (void)RCC->AHB1ENR;
}

/*********************************************************************
 * @fn      		  - GPIO_ConfigOutputPP
 * @brief             - Configure pin as push-pull output, low speed, no pull
 * @param[in]         - pGPIOx : GPIO port base address
 * @param[in]         - pin    : pin number (0-15)
 * @return            - none
 * @Note              - Bit-clear-before-set is mandatory here: MODER is a
 *                       2-bit field, so OR-only would corrupt the field if
 *                       it was previously set to another mode (e.g. AF=10b).
 *////////////////////////////////////////////////////////////////////
void GPIO_ConfigOutputPP(GPIO_TypeDef * const pGPIOx, uint8_t pin)
{
    uint32_t const shift = (uint32_t)pin * 2U;

    /* Mode -> General purpose output */
    pGPIOx->MODER &= ~(GPIO_MODER_MASK << shift);
    pGPIOx->MODER |= (GPIO_MODER_OUTPUT << shift);

    /* Output type -> Push-pull (bit is single-bit per pin, clear = push-pull) */
    pGPIOx->OTYPER &= ~(1UL << pin);

    /* Speed -> Low speed (00b), clear only, no set needed */
    pGPIOx->OSPEEDR &= ~(GPIO_MODER_MASK << shift);

    /* Pull -> No pull */
    pGPIOx->PUPDR &= ~(GPIO_PUPDR_MASK << shift);
}

/*********************************************************************
 * @fn      		  - GPIO_ConfigInput
 * @brief             - Configure pin as input with selectable pull resistor
 * @param[in]         - pGPIOx : GPIO port base address
 * @param[in]         - pin    : pin number (0-15)
 * @param[in]         - pull   : GPIO_PULL_NONE / GPIO_PULL_UP / GPIO_PULL_DOWN
 * @return            - none
 * @Note              - MODER=00b is the reset value for input, but we still
 *                       clear explicitly rather than relying on reset state,
 *                       in case this pin was reconfigured at runtime.
 *////////////////////////////////////////////////////////////////////
void GPIO_ConfigInput(GPIO_TypeDef * const pGPIOx, uint8_t pin, GPIO_PullType_t pull)
{
    uint32_t const shift = (uint32_t)pin * 2U;

    pGPIOx->MODER &= ~(GPIO_MODER_MASK << shift);
    pGPIOx->MODER |= (GPIO_MODER_INPUT << shift);

    pGPIOx->PUPDR &= ~(GPIO_PUPDR_MASK << shift);
    pGPIOx->PUPDR |= ((uint32_t)pull << shift);
}

/*********************************************************************
 * @fn      		  - GPIO_SetPin / GPIO_ClearPin / GPIO_TogglePin
 * @brief             - Atomic pin set/clear via BSRR (preferred over ODR
 *                       read-modify-write, which is not atomic and can race
 *                       with an ISR touching the same port).
 *////////////////////////////////////////////////////////////////////
void GPIO_SetPin(GPIO_TypeDef * const pGPIOx, uint8_t pin)
{
    pGPIOx->BSRR = (1UL << pin);
}

void GPIO_ClearPin(GPIO_TypeDef * const pGPIOx, uint8_t pin)
{
    pGPIOx->BSRR = (1UL << (pin + 16U));
}

void GPIO_TogglePin(GPIO_TypeDef * const pGPIOx, uint8_t pin)
{
    if ((pGPIOx->ODR & (1UL << pin)) != 0U)
    {
        GPIO_ClearPin(pGPIOx, pin);
    }
    else
    {
        GPIO_SetPin(pGPIOx, pin);
    }
}

/*********************************************************************
 * @fn      		  - GPIO_ReadPin
 * @brief             - Read current logic level of an input pin
 * @return            - GPIO_PIN_STATE_HIGH or GPIO_PIN_STATE_LOW
 *////////////////////////////////////////////////////////////////////
uint8_t GPIO_ReadPin(const GPIO_TypeDef * const pGPIOx, uint8_t pin)
{
    uint8_t state;

    if ((pGPIOx->IDR & (1UL << pin)) != 0U)
    {
        state = GPIO_PIN_STATE_HIGH;
    }
    else
    {
        state = GPIO_PIN_STATE_LOW;
    }

    return state;
}
