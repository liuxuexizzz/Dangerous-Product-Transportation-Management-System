#include "./BSP/adc_dma/adc_dma.h"
#include "./SYSTEM/delay/delay.h"

DMA_HandleTypeDef g_dma_nch_adc_handle;
ADC_HandleTypeDef g_adc_nch_dma_handle;
uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];
uint8_t g_adc_dma_sta;


/* ADC N通道(1通道) DMA读取 初始化函数 */
void adc_nch_dma_init(uint32_t mar)
{
    ADC_ChannelConfTypeDef adc_ch_conf;
    
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    g_dma_nch_adc_handle.Instance = DMA1_Channel1;
    g_dma_nch_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    g_dma_nch_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_dma_nch_adc_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_dma_nch_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    g_dma_nch_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    g_dma_nch_adc_handle.Init.Mode = DMA_NORMAL;
    g_dma_nch_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;
    HAL_DMA_Init(&g_dma_nch_adc_handle);
    
    __HAL_LINKDMA(&g_adc_nch_dma_handle, DMA_Handle, g_dma_nch_adc_handle);

    g_adc_nch_dma_handle.Instance = ADC1;
    g_adc_nch_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    g_adc_nch_dma_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;
    g_adc_nch_dma_handle.Init.ContinuousConvMode = ENABLE;
    g_adc_nch_dma_handle.Init.NbrOfConversion = 1;     // 设置通道个数为1
    g_adc_nch_dma_handle.Init.DiscontinuousConvMode = DISABLE;
    g_adc_nch_dma_handle.Init.NbrOfDiscConversion = 0;
    g_adc_nch_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    HAL_ADC_Init(&g_adc_nch_dma_handle);
    
    HAL_ADCEx_Calibration_Start(&g_adc_nch_dma_handle);

    //通道1
    adc_ch_conf.Channel = ADC_CHANNEL_0;
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf);
    
/*  //通道2
    adc_ch_conf.Channel = ADC_CHANNEL_1;
    adc_ch_conf.Rank = ADC_REGULAR_RANK_2;
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf);
*/
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 3);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    HAL_DMA_Start_IT(&g_dma_nch_adc_handle, (uint32_t)&ADC1->DR, mar, 0);
    HAL_ADC_Start_DMA(&g_adc_nch_dma_handle, &mar ,0);
}

/* ADC MSP初始化函数 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == ADC1)
    {
        GPIO_InitTypeDef gpio_init_struct;
        RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
        
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_ADC1_CLK_ENABLE();

        gpio_init_struct.Pin = GPIO_PIN_0; /* 添加通道需添加对应引脚, 格式 | GPIO_PIN_1;  */
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA, &gpio_init_struct); 
        
        adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;
        adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;
        HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);
    }
}

/* 使能一次ADC DMA传输函数 */
void adc_dma_enable(uint16_t cndtr)
{
    ADC1->CR2 &= ~(1 << 0);
    
    DMA1_Channel1->CCR &= ~(1 << 0);
    while (DMA1_Channel1->CCR & (1 << 0));
    DMA1_Channel1->CNDTR = cndtr;
    DMA1_Channel1->CCR |= 1 << 0;

    ADC1->CR2 |= 1 << 0;
    ADC1->CR2 |= 1 << 22;
}


/* ADC DMA采集中断服务函数 */
void DMA1_Channel1_IRQHandler(void)
{
    if (DMA1->ISR & (1<<1))
    {
        g_adc_dma_sta = 1;
        DMA1->IFCR |= 1 << 1;
    }
}


/* AD值换算电压函数， map（AD值，AD最小值，AD最大值，电压最小值，电压最大值） */
long map(long x, long in_min, long in_max, long out_min, long out_max) 
{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


/* 获得通道a的adc平均值函数, a 为第n个通道，n为正整数*/
uint16_t Get_Adc_Average(int a) 
{
		uint16_t i,j;
		uint16_t adcx;
    uint16_t adc_value;
		uint32_t sum;
    if (g_adc_dma_sta == 1)
    {
        /* 循环计算通道0~通道n的结果 */
        for(j = 0; j < 1; j++)                          /* 遍历n个通道 */
        {
            sum = 0; 																		/* 清零 */
            for (i = 0; i < ADC_DMA_BUF_SIZE / 1; i++)  /* 每个通道采集了20次数据,进行20次累加 */
            {
                sum += g_adc_dma_buf[(1 * i) + j];      /* 相同通道的转换数据累加 */
            }
            adcx = sum / (ADC_DMA_BUF_SIZE / 1);        /* 取平均值 */
						if (j == a)
						{
								adc_value = adcx;
				                                                                          				}
						}
		}
		g_adc_dma_sta = 0;                      /* 清除DMA采集完成状态标志 */
    adc_dma_enable(ADC_DMA_BUF_SIZE);       /* 启动下一次ADC DMA采集 */
		delay_ms(50);
		return adc_value;
}


