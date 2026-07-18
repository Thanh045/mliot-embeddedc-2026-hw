#include "stm32f1xx_hal.h"

#define LED_PORT        GPIOC
#define LED_PIN         GPIO_PIN_13

#define BUTTON_PORT     GPIOA
#define BUTTON_PIN      GPIO_PIN_0

#define DEBOUNCE_MS     25U

/*
 * Trả về 1 đúng một lần sau khi nút đã được nhấn rồi thả.
 * PA0 dùng pull-up:
 * - Không nhấn: GPIO_PIN_SET
 * - Nhấn:      GPIO_PIN_RESET
 */
static uint8_t Button_ReleaseEvent(void)
{
    static GPIO_PinState previous_raw_state = GPIO_PIN_SET;
    static GPIO_PinState stable_state = GPIO_PIN_SET;
    static uint32_t last_change_time = 0U;
    static uint8_t press_detected = 0U;

    GPIO_PinState current_raw_state =
        HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN);

    uint32_t current_time = HAL_GetTick();

    /* Tín hiệu thô vừa thay đổi */
    if (current_raw_state != previous_raw_state)
    {
        previous_raw_state = current_raw_state;
        last_change_time = current_time;
    }

    /* Chưa ổn định đủ thời gian debounce */
    if ((current_time - last_change_time) < DEBOUNCE_MS)
    {
        return 0U;
    }

    /* Trạng thái ổn định vừa thay đổi */
    if (stable_state != current_raw_state)
    {
        stable_state = current_raw_state;

        /* Nút được nhấn */
        if (stable_state == GPIO_PIN_RESET)
        {
            press_detected = 1U;
        }
        /* Nút được thả sau khi đã nhấn */
        else if (press_detected != 0U)
        {
            press_detected = 0U;
            return 1U;
        }
    }

    return 0U;
}

int main(void)
{
    GPIO_InitTypeDef gpio = {0};

    HAL_Init();

    /* Bật clock trước khi cấu hình GPIO */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /*
     * LED PC13 của Blue Pill là active-low:
     * SET   = tắt
     * RESET = sáng
     */
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);

    gpio.Pin = LED_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &gpio);

    /* PA0 làm input pull-up */
    gpio.Pin = BUTTON_PIN;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BUTTON_PORT, &gpio);

    while (1)
    {
        if (Button_ReleaseEvent() != 0U)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        }
    }
}