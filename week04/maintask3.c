#include "stm32f1xx_hal.h"

#define LED_PORT GPIOC
#define LED_PIN  GPIO_PIN_13

int main(void)
{
    GPIO_InitTypeDef gpio = {0};

    HAL_Init();

    /* Bắt buộc bật clock GPIOC trước khi cấu hình PC13 */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* LED Blue Pill ở PC13 là active-low */
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);

    gpio.Pin = LED_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(LED_PORT, &gpio);

    while (1)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        HAL_Delay(500);
    }
}