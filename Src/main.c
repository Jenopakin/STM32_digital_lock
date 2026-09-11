/*******************************************************************************
 * File Name    : main.c
 * Description  : Week 1 bring-up test - verify button edge capture + timing
 *                accuracy. NOT the final application (no SHORT/LONG decode,
 *                no state machine yet - that begins Week 2).
 * Date         : 2026-09-06
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f411xe.h"
#include "gpio_driver.h"
#include "exti_driver.h"
#include "timer_driver.h"
#include <stdint.h>

/* Private define --------------------------------------------------------------*/
#define LED_PIN         (5U)   /* PA5, on-board user LED per Nucleo pinout */
#define TEST_THRESHOLD_MS (400UL) /* rough short/long cut for visual test only */

/* Private variables ------------------------------------------------------------*/
static volatile uint32_t s_pressStartTick = 0UL;
static volatile uint32_t s_lastDurationMs  = 0UL;
static volatile uint8_t  s_newResultReady  = 0U;

/* Private function prototypes --------------------------------------------------*/
static void Button_EdgeCallback(uint8_t pinState);
static void BlinkLed(uint8_t times);

/* Main function -------------------------------------------------------------*/
int main(void)
{
    GPIO_EnableClock(GPIOA);
    GPIO_ConfigOutputPP(GPIOA, LED_PIN);

    TIMER_Init();
    EXTI_Button_Init();
    EXTI_Button_RegisterCallback(Button_EdgeCallback);

    while (1)
    {
        if (s_newResultReady == 1U)
        {
            s_newResultReady = 0U;

            /* Test-only visual feedback:
             * 1 blink = classified SHORT, 2 blinks = classified LONG.
             * Real classification module (code_decoder) arrives Week 2. */
            if (s_lastDurationMs < TEST_THRESHOLD_MS)
            {
                BlinkLed(1U);
            }
            else
            {
                BlinkLed(2U);
            }
        }
    }
}

/*********************************************************************
 * @fn      		  - Button_EdgeCallback
 * @brief             - Called by exti_driver on every press/release edge.
 *                       pinState == 0 -> pressed (active-low with pull-up)
 *                       pinState == 1 -> released
 *////////////////////////////////////////////////////////////////////
static void Button_EdgeCallback(uint8_t pinState)
{
    if (pinState == GPIO_PIN_STATE_LOW)
    {
        /* falling edge: press started */
        s_pressStartTick = TIMER_GetTick();
    }
    else
    {
        /* rising edge: press ended, compute duration */
        s_lastDurationMs = TIMER_GetTick() - s_pressStartTick;
        s_newResultReady = 1U;
    }
}

/*********************************************************************
 * @fn      		  - BlinkLed
 * @brief             - Test-only helper: blink LED N times using the
 *                       blocking TIMER_DelayMs(). Fine for a bring-up test
 *                       running in the main loop (not inside an ISR).
 *////////////////////////////////////////////////////////////////////
static void BlinkLed(uint8_t times)
{
    uint8_t i;

    for (i = 0U; i < times; i++)
    {
        GPIO_SetPin(GPIOA, LED_PIN);
        TIMER_DelayMs(150UL);
        GPIO_ClearPin(GPIOA, LED_PIN);
        TIMER_DelayMs(150UL);
    }
}
