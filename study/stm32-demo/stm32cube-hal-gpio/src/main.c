#include "stm32f1xx_hal.h"

#define LED_PIN GPIO_PIN_13
#define LED_GPIO_PORT GPIOC
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()

#define KEY_PIN GPIO_PIN_0
#define KEY_GPIO_PORT GPIOA
#define KEY_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

int main(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    ret = HAL_Init();
    if (ret != HAL_OK)
    {
        goto END;
    }

    LED_GPIO_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct2;

    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

    /* --------------------------------------------------------------- */
    /* 读取 KEY PA0 的 值 决定是否开启 blink 程序 */
    KEY_GPIO_CLK_ENABLE();

    GPIO_InitStruct2.Pin = KEY_PIN;
    GPIO_InitStruct2.Mode = GPIO_MODE_INPUT; //输入
    GPIO_InitStruct2.Pull = GPIO_PULLUP;
    GPIO_InitStruct2.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct2);

    while (1)
    {
        if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_PIN))
            break;

        HAL_Delay(5);
    }
    /* --------------------------------------------------------------- */

    while (1)
    {
        /* blink method 1 */
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);
        HAL_Delay(10);
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET);
        HAL_Delay(10);

        /* blink method 2 */
        HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
        HAL_Delay(10);
    }

END:
    HAL_GPIO_DeInit(LED_GPIO_PORT, LED_PIN);
    HAL_DeInit();
}

void HAL_MspInit(void)
{
}

void HAL_MspDeInit(void)
{
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1)
    {
    }
}

void MemManage_Handler(void)
{
    while (1)
    {
    }
}

void BusFault_Handler(void)
{
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}
