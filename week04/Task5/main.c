#include "main.h"

/* ==================== Khai báo chân keypad ==================== */

#define ROW1_PORT      GPIOB
#define ROW1_PIN       GPIO_PIN_0

#define ROW2_PORT      GPIOB
#define ROW2_PIN       GPIO_PIN_1

#define COL1_PORT      GPIOB
#define COL1_PIN       GPIO_PIN_10

#define COL2_PORT      GPIOB
#define COL2_PIN       GPIO_PIN_11

/* ==================== Khai báo chân LED ==================== */

#define LED1_PORT      GPIOB
#define LED1_PIN       GPIO_PIN_2

#define LED2_PORT      GPIOB
#define LED2_PIN       GPIO_PIN_3

#define LED3_PORT      GPIOB
#define LED3_PIN       GPIO_PIN_4

#define LED4_PORT      GPIOB
#define LED4_PIN       GPIO_PIN_5

/* ==================== Prototype ==================== */

static void MX_GPIO_Init(void);
static uint8_t Keypad_Scan(void);
static void Toggle_LED(uint8_t key);
static void All_Rows_High(void);
static void All_LEDs_Off(void);

/* ============================================================ */

int main(void)
{
    uint8_t currentKey;
    uint8_t previousKey = 0;

    HAL_Init();

    /*
     * Không bắt buộc gọi SystemClock_Config() trong ví dụ này.
     * Vi điều khiển sử dụng clock mặc định sau reset.
     */

    MX_GPIO_Init();

    /* Ban đầu tắt toàn bộ LED */
    All_LEDs_Off();

    /* Ban đầu đưa hai hàng lên HIGH */
    All_Rows_High();

    while (1)
    {
        currentKey = Keypad_Scan();

        /*
         * Chỉ toggle khi phát hiện một lần nhấn mới:
         * previousKey = 0 và currentKey khác 0.
         */
        if ((currentKey != 0) && (previousKey == 0))
        {
            /* Chống dội phím */
            HAL_Delay(20);

            /* Kiểm tra lại sau 20 ms */
            if (Keypad_Scan() == currentKey)
            {
                Toggle_LED(currentKey);
            }
        }

        previousKey = currentKey;

        HAL_Delay(5);
    }
}

/* ============================================================
 * Quét keypad 2x2
 *
 * Giá trị trả về:
 * 0: Không có phím được nhấn
 * 1: Phím R1-C1
 * 2: Phím R1-C2
 * 3: Phím R2-C1
 * 4: Phím R2-C2
 * ============================================================ */

static uint8_t Keypad_Scan(void)
{
    /* ==================== Quét hàng 1 ==================== */

    /*
     * R1 = LOW
     * R2 = HIGH
     */
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);

    /*
     * Chờ tín hiệu ổn định.
     * Có thể dùng vài micro giây, nhưng 1 ms vẫn phù hợp bài thực hành.
     */
    HAL_Delay(1);

    /* Phím 1: R1-C1 */
    if (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET)
    {
        All_Rows_High();
        return 1;
    }

    /* Phím 2: R1-C2 */
    if (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET)
    {
        All_Rows_High();
        return 2;
    }

    /* ==================== Quét hàng 2 ==================== */

    /*
     * R1 = HIGH
     * R2 = LOW
     */
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_RESET);

    HAL_Delay(1);

    /* Phím 3: R2-C1 */
    if (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET)
    {
        All_Rows_High();
        return 3;
    }

    /* Phím 4: R2-C2 */
    if (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET)
    {
        All_Rows_High();
        return 4;
    }

    /* Không có phím được nhấn */
    All_Rows_High();

    return 0;
}

/* ============================================================
 * Đổi trạng thái LED tương ứng
 *
 * LED đang sáng  -> tắt
 * LED đang tắt   -> sáng
 * ============================================================ */

static void Toggle_LED(uint8_t key)
{
    switch (key)
    {
        case 1:
            HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN);
            break;

        case 2:
            HAL_GPIO_TogglePin(LED2_PORT, LED2_PIN);
            break;

        case 3:
            HAL_GPIO_TogglePin(LED3_PORT, LED3_PIN);
            break;

        case 4:
            HAL_GPIO_TogglePin(LED4_PORT, LED4_PIN);
            break;

        default:
            break;
    }
}

/* Đưa tất cả hàng của keypad lên HIGH */

static void All_Rows_High(void)
{
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);
}

/* Tắt toàn bộ LED vì LED active-low */

static void All_LEDs_Off(void)
{
    HAL_GPIO_WritePin(
        GPIOB,
        LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN,
        GPIO_PIN_SET
    );
}

/* ============================================================
 * Khởi tạo GPIO
 * ============================================================ */

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Bật clock cho GPIOB */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* --------------------------------------------------------
     * Cấu hình PB0 và PB1 làm Output cho hai hàng keypad
     * -------------------------------------------------------- */

    HAL_GPIO_WritePin(
        GPIOB,
        ROW1_PIN | ROW2_PIN,
        GPIO_PIN_SET
    );

    GPIO_InitStruct.Pin = ROW1_PIN | ROW2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* --------------------------------------------------------
     * Cấu hình PB10 và PB11 làm Input Pull-up cho hai cột
     * -------------------------------------------------------- */

    GPIO_InitStruct.Pin = COL1_PIN | COL2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* --------------------------------------------------------
     * Cấu hình PB2, PB3, PB4, PB5 làm Output cho LED
     * -------------------------------------------------------- */

    /*
     * Đưa GPIO lên HIGH trước để LED không sáng
     * ngay lúc vừa khởi động.
     */
    HAL_GPIO_WritePin(
        GPIOB,
        LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN,
        GPIO_PIN_SET
    );

    GPIO_InitStruct.Pin =
        LED1_PIN |
        LED2_PIN |
        LED3_PIN |
        LED4_PIN;

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}