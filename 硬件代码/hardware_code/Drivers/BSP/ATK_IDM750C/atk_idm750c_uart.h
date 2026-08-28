/**
 ****************************************************************************************************
 * @file        atk_idm750c_uart.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK_IDM750C模块UART接口驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 精英版 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __ATK_IDM750C_UART_H
#define __ATK_IDM750C_UART_H

#include "./SYSTEM/sys/sys.h"

/* 引脚定义 */
#define ATK_IDM750C_UART_TX_GPIO_PORT         GPIOA
#define ATK_IDM750C_UART_TX_GPIO_PIN          GPIO_PIN_2
#define ATK_IDM750C_UART_TX_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)

#define ATK_IDM750C_UART_RX_GPIO_PORT         GPIOA
#define ATK_IDM750C_UART_RX_GPIO_PIN          GPIO_PIN_3
#define ATK_IDM750C_UART_RX_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)

#define ATK_IDM750C_UART_INTERFACE            USART2
#define ATK_IDM750C_UART_IRQn                 USART2_IRQn
#define ATK_IDM750C_UART_IRQHandler           USART2_IRQHandler
#define ATK_IDM750C_UART_CLK_ENABLE()         do{ __HAL_RCC_USART2_CLK_ENABLE(); }while(0)

/* UART收发缓冲大小 */
#define ATK_IDM750C_UART_RX_BUF_SIZE          1024
#define ATK_IDM750C_UART_TX_BUF_SIZE          512

/* 操作函数 */
void atk_idm750c_uart_printf(char *fmt, ...);           /* ATK-IDM750C UART printf */
void atk_idm750c_uart_rx_restart(void);                 /* ATK-IDM750C UART重新开始接收数据 */
uint8_t *atk_idm750c_uart_rx_get_frame(void);           /* 获取ATK-IDM750C UART接收到的一帧数据 */
uint16_t atk_idm750c_uart_rx_get_frame_len(void);       /* 获取ATK-IDM750C UART接收到的一帧数据的长度 */
void atk_idm750c_uart_init(uint32_t baudrate);          /* ATK-IDM750C UART初始化 */

#endif
