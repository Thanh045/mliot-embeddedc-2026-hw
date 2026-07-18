#include "stm32f1xx_hal.h"

#define ROW_PORT          GPIOB
#define ROW_PINS          (GPIO_PIN_0 | GPIO_PIN_1)

#define COL_PORT          GPIOB
#define COL_PINS          (GPIO_PIN_10 | GPIO_PIN_11)

#define FEEDBACK_LED_PORT GPIOC
#define FEEDBACK_LED_PIN  GPIO_PIN_13

#define DEBOUNCE_MS       25U

typedef enum
{
    KEY_NONE = 0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4
} KeypadKey;

static KeypadKey Keypad_ScanRaw(void)
{
    static const uint16_t row_pins[2] = {GPIO_PIN_0, GPIO_PIN_1};
    static const uint16_t col_pins[2] = {GPIO_PIN_10, GPIO_PIN_11};

    uint32_t row, col;

    for (row = 0; row < 2; row++)
    {
        HAL_GPIO_WritePin(ROW_PORT, ROW_PINS, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ROW_PORT, row_pins[row], GPIO_PIN_RESET);

        __NOP();
        __NOP();
        __NOP();

        for (col = 0; col < 2; col++)
        {
            if (HAL_GPIO_ReadPin(COL_PORT, col_pins[col]) == GPIO_PIN_RESET)
            {
                HAL_GPIO_WritePin(ROW_PORT, ROW_PINS, GPIO_PIN_SET);
                return (KeypadKey)(row * 2 + col + 1);
            }
        }
    }

    HAL_GPIO_WritePin(ROW_PORT, ROW_PINS, GPIO_PIN_SET);
    return KEY_NONE;
}

static KeypadKey Keypad_PressEvent(void)
{
    static KeypadKey candidate = KEY_NONE;
    static uint32_t candidate_since = 0;
    static uint8_t released = 1;

    KeypadKey raw = Keypad_ScanRaw();
    uint32_t now = HAL_GetTick();

    if (raw != candidate)
    {
        candidate = raw;
        candidate_since = now;
        return KEY_NONE;
    }

    if ((now - candidate_since) < DEBOUNCE_MS)
        return KEY_NONE;

    if (candidate == KEY_NONE)
    {
        released = 1;
        return KEY_NONE;
    }

    if (released)
    {
        released = 0;
        return candidate;
    }

    return KEY_NONE;
}

int main(void)
{
    GPIO_InitTypeDef gpio = {0};

    HAL_Init();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* ROW: PB0 PB1 */
    HAL_GPIO_WritePin(ROW_PORT, ROW_PINS, GPIO_PIN_SET);

    gpio.Pin = ROW_PINS;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ROW_PORT, &gpio);

    /* COLUMN: PB10 PB11 */
    gpio.Pin = COL_PINS;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(COL_PORT, &gpio);

    /* LED PC13 */
    HAL_GPIO_WritePin(FEEDBACK_LED_PORT,
                      FEEDBACK_LED_PIN,
                      GPIO_PIN_SET);   // LED tắt

    gpio.Pin = FEEDBACK_LED_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(FEEDBACK_LED_PORT, &gpio);

    while (1)
    {
        if (Keypad_PressEvent() != KEY_NONE)
        {
            HAL_GPIO_TogglePin(FEEDBACK_LED_PORT,
                               FEEDBACK_LED_PIN);
        }
    }
}