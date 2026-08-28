#ifndef __ADCDMA_H
#define __ADCDMA_H	

#include "./SYSTEM/sys/sys.h"

#define VOLTAGE_MIN 100          				  		 		/* 引脚电压限量程范围，最小值100mV */
#define VOLTAGE_MAX 3300                    			/* 引脚电压限量程范围，最大值3300mV */
#define ADC_DMA_BUF_SIZE        20 * 1      			/* ADC DMA采集内存缓冲区大小, 应等于ADC通道数的整数倍 */
#define PRESS_MIN	20      												/* 最小量程 根据具体型号对应手册获取,单位g，这里以RP-18.3-ST型号为例，最小量程是20g */
#define PRESS_MAX	6000    												/* 最大量程 根据具体型号对应手册获取,单位g，这里以RP-18.3-ST型号为例，最大量程是6kg */

extern uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];					/* 缓冲大小 */
extern uint8_t g_adc_dma_sta;               							/* 中断服务DMA传输状态标志, 0,未完成; 1, 已完成 */

uint16_t Get_Adc_Average(int a);
void adc_nch_dma_init(uint32_t mar);
void adc_dma_enable(uint16_t cndtr);
long map(long x, long in_min, long in_max, long out_min, long out_max);

#endif 

