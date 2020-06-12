/* 
 * 
 * Developed by Timofei Nikitin <tim.a.nikitin@yandex.ru>
 * 
 * 
 * Module for image receiving and rendering on the E-Ink display
 * 
 * app_init()     - Initializes image reception: enables reception of the initial byte
 *                  Called before an infinite loop in the main()
 * 
 * app_periodic() - Called every time in an infinite loop
 * 
 */

#include "app.h"

extern UART_HandleTypeDef huart1;
extern CRC_HandleTypeDef hcrc;

// Declaration static function

// Static variables
static uint8_t picture[PICTURE_SIZE]; //Image storage buffer
static uint16_t picture_i = 0;
static uint32_t picture_crc;

static volatile bool read_bytes_done = false;
static uint8_t rx_byte;
static uint32_t rx_timeout = 0;

static app_state state = IDLE;

void app_init(void)
{
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

void app_periodic(void)
{
    switch (state) {
        // IDLE state - wait for the start byte to be received, 
        // indicating the start of the transfer
        case IDLE :
            if (read_bytes_done) {
                read_bytes_done = false;
                if (rx_byte == CMD_START) {
                    // Set the iterator to zero and pass it
                    picture_i = 0;
                    HAL_UART_Transmit(&huart1, &picture_i, 1, 1000);
                    // We start receiving a packet
                    HAL_UART_Receive_IT(&huart1, &picture[picture_i*PICTURE_PACKET_LEN], PICTURE_PACKET_LEN);
                    // Go to READ state
                    state = READ;
                    // The LED indicates that the transmission has begun
                    HAL_GPIO_WritePin(LD_R_GPIO_Port, LD_R_Pin, GPIO_PIN_SET);
                    // Set Timeout
                    rx_timeout = HAL_GetTick() + RX_TIMEOUT;
                }
                else {
                    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
                }
            }
            break;
        // READ state - accept the remaining parts of the image
        case READ :
            // If the timeout has not yet expired, save the packet
            if (rx_timeout >= HAL_GetTick()) {
                if (read_bytes_done) {
                    read_bytes_done = false;
                    picture_i++;
                    // If the last packet is accepted
                    if (picture_i >= PICTURE_WIDTH) {
                        // We calculate CRC and send it
                        picture_crc = HAL_CRC_Calculate(&hcrc, (uint32_t *)picture, PICTURE_SIZE);
                        HAL_UART_Transmit(&huart1, (uint8_t *) &picture_crc, 4, 1000);
                        // Go to CHECK state
                        picture_i = 0;
                        state = CHECK;
                        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
                    }
                    // Otherwise, we configure the reception of the next packet
                    else {
                        HAL_UART_Transmit(&huart1, &picture_i, 1, 1000);
                        HAL_UART_Receive_IT(&huart1, &picture[picture_i*PICTURE_PACKET_LEN], PICTURE_PACKET_LEN);
                    }
                    rx_timeout = HAL_GetTick() + RX_TIMEOUT;
                }
            }
            // Otherwise, we reset everything
            else {
                HAL_UART_AbortReceive_IT(&huart1);
                state = IDLE;
                read_bytes_done = false;
                HAL_GPIO_WritePin(LD_R_GPIO_Port, LD_R_Pin, GPIO_PIN_RESET);
                HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
            }
            break;
        // CHECK state - expect CRC check result
        case CHECK :
            // If the wait time has not yet expired
            if (rx_timeout >= HAL_GetTick()) {
                if (read_bytes_done) {
                    read_bytes_done = false;
                    // If the CRC matches, the CMD_END command should come and then go to the next state
                    if (rx_byte == CMD_END) {
                        state = DRAW;
                    }
                    // Owerwise, go to IDLE state
                    else {
                        state = IDLE;
                    }
                    HAL_GPIO_WritePin(LD_R_GPIO_Port, LD_R_Pin, GPIO_PIN_RESET);
                    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
                }
            }
            // Otherwise, we reset everything
            else {
                HAL_UART_AbortReceive_IT(&huart1);
                state = IDLE;
                read_bytes_done = false;
                HAL_GPIO_WritePin(LD_R_GPIO_Port, LD_R_Pin, GPIO_PIN_RESET);
                HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
            }
            break;
        // CHECK state - rendering of the received image
        case DRAW :
            BSP_EPD_Init();
            BSP_EPD_Clear(EPD_COLOR_WHITE);
            BSP_EPD_DrawImage(0, 0, PICTURE_HEIGHT, PICTURE_WIDTH, picture);
            BSP_EPD_RefreshDisplay();
            BSP_EPD_CloseChargePump();
            state = IDLE;
            break;
        // In unforeseen states, go into IDLE state
        default:
            read_bytes_done = false;
            HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
            state = IDLE;
            break;
    }
}

// Static function

// Callbacks
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    HAL_GPIO_TogglePin(LD_G_GPIO_Port, LD_G_Pin);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    read_bytes_done = true;
}
