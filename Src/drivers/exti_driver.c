/*******************************************************************************
 * File Name    : exti_driver.c
 * Description  : EXTI4 driver for push button on PB4 (both-edge triggered)
 * Date         : 2026-09-06
 ******************************************************************************/

#include "exti_driver.h"
#include "gpio_driver.h"

/* Private define ---------------------------------------------------------------
 * SYSCFG->EXTICR[1] (EXTICR2) bits[3:0] select the port for EXTI line 4.
 * Value 0x1 = GPIOB (per SYSCFG_EXTICR2 register mapping)
 * -------------------------------------------------------------------------------*/
#define BUTTON_PIN          (4U)
#define EXTICR_LINE4_PORTB  (0x1UL)
#define EXTICR2_LINE4_SHIFT (0U)
#define EXTICR2_LINE4_MASK  (0xFUL)

/* Private variables --------------------------------------------------------------*/
static EXTI_ButtonCallback_t s_callback = (EXTI_ButtonCallback_t)0;

/* Public functions -----------------------------------------------------------*/

/*********************************************************************
 * @fn      		  - EXTI_Button_Init
 * @brief             - Configure PB4 as input (pull-up) and enable EXTI4
 *                       interrupt on both rising and falling edge
 * @Note              - Pull-up assumes the button on this shield is
 *                       active-low (pressed = 0). VERIFY against the actual
 *                       STEO shield revision in use (blue vs red silkscreen)
 *                       before trusting polarity - invert in the callback
 *                       if this board wires it active-high instead.
 *////////////////////////////////////////////////////////////////////
void EXTI_Button_Init(void)
{
    /* 1. GPIO: PB4 as input with pull-up */
    GPIO_EnableClock(GPIOB);
    GPIO_ConfigInput(GPIOB, BUTTON_PIN, GPIO_PULL_UP);

    /* 2. SYSCFG: route EXTI line 4 to GPIOB */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    (void)RCC->APB2ENR;

    SYSCFG->EXTICR[1] &= ~(EXTICR2_LINE4_MASK << EXTICR2_LINE4_SHIFT);
    SYSCFG->EXTICR[1] |= (EXTICR_LINE4_PORTB << EXTICR2_LINE4_SHIFT);

    /* 3. EXTI: unmask line 4, trigger on both edges */
    EXTI->IMR  |= (1UL << BUTTON_PIN);
    EXTI->RTSR |= (1UL << BUTTON_PIN);
    EXTI->FTSR |= (1UL << BUTTON_PIN);

    /* Clear any stale pending flag left over from configuration transients */
    EXTI->PR = (1UL << BUTTON_PIN);

    /* 4. NVIC */
    NVIC_SetPriority(EXTI4_IRQn, 1U);
    NVIC_EnableIRQ(EXTI4_IRQn);
}

/*********************************************************************
 * @fn      		  - EXTI_Button_RegisterCallback
 * @brief             - Register the app-layer function to be called on
 *                       every button edge. Keeps this driver ignorant of
 *                       what SHORT/LONG/state-machine logic even is.
 *////////////////////////////////////////////////////////////////////
void EXTI_Button_RegisterCallback(EXTI_ButtonCallback_t callback)
{
    s_callback = callback;
}

/*********************************************************************
 * @fn      		  - EXTI4_IRQHandler
 * @brief             - ISR for PB4 edge event
 * @Note              - EXTI->PR must be cleared manually by writing 1 to
 *                       the corresponding bit (write-1-to-clear), unlike
 *                       UART RXNE/ADC EOC which clear on register read.
 *////////////////////////////////////////////////////////////////////
void EXTI4_IRQHandler(void)
{
    if ((EXTI->PR & (1UL << BUTTON_PIN)) != 0U)
    {
        EXTI->PR = (1UL << BUTTON_PIN); /* clear pending bit first */

        if (s_callback != (EXTI_ButtonCallback_t)0)
        {
            uint8_t const pinState = GPIO_ReadPin(GPIOB, BUTTON_PIN);
            s_callback(pinState);
        }
    }
}
