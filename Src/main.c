#include "stm32f411xe.h"
#include "gpio_driver.h"
#include "exti_driver.h"
#include "timer_driver.h"
#include "uart_debug.h"
#include "code_decoder.h"
#include <stdint.h>
#include <stdio.h>

#define LED_PIN  (5U)

static volatile uint32_t s_pressStartTick = 0UL;
static volatile uint32_t s_lastDurationMs  = 0UL;
static volatile uint8_t  s_newResultReady  = 0U;

static void Button_EdgeCallback(uint8_t pinState);
static void BlinkLed(uint8_t times);

int main(void)
{
    char msg[64];

    GPIO_EnableClock(GPIOA);
    GPIO_ConfigOutputPP(GPIOA, LED_PIN);

    TIMER_Init();
    UART_Debug_Init();
    EXTI_Button_Init();
    EXTI_Button_RegisterCallback(Button_EdgeCallback);

    UART_Debug_Print("=== Week 1 Timing Test Ready ===\r\n");

    while (1)
    {
        if (s_newResultReady == 1U)
        {
            Symbol_t symbol;

            s_newResultReady = 0U;
            symbol = CodeDecoder_Classify(s_lastDurationMs);

            if (symbol == SYMBOL_SHORT)
            {
                (void)sprintf(msg, "Duration = %lu ms -> SHORT\r\n",
                              (unsigned long)s_lastDurationMs);
                BlinkLed(1U);
            }
            else
            {
                (void)sprintf(msg, "Duration = %lu ms -> LONG\r\n",
                              (unsigned long)s_lastDurationMs);
                BlinkLed(2U);
            }

            UART_Debug_Print(msg);
        }
    }
}

static void Button_EdgeCallback(uint8_t pinState)
{
    if (pinState == GPIO_PIN_STATE_LOW)
    {
        s_pressStartTick = TIMER_GetTick();
    }
    else
    {
        s_lastDurationMs = TIMER_GetTick() - s_pressStartTick;
        s_newResultReady = 1U;
    }
}

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
