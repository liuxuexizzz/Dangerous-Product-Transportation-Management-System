#include "./BSP/adc/adc.h"
#include "./SYSTEM/delay/delay.h"


ADC_HandleTypeDef g_adc_handle;   /* ADC句柄 */

/**
 * @brief       ADC初始化函数
 *   @note      本函数支持ADC1/ADC2任意通道, 但是不支持ADC3
 *              我们使用12位精度, ADC采样时钟=12M, 转换时间为: 采样周期 + 12.5个ADC周期
 *              设置最大采样周期: 239.5, 则转换时间 = 252 个ADC周期 = 21us
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
    g_adc_handle.Instance = ADC_ADCX;                        /* 选择哪个ADC */
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;       /* 数据对齐方式：右对齐 */
    g_adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;       /* 非扫描模式，仅用到一个通道 */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;          /* 关闭连续转换模式 */
    g_adc_handle.Init.NbrOfConversion = 1;                   /* 赋值范围是1~16，本实验用到1个规则通道序列 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;       /* 禁止规则通道组间断模式 */
    g_adc_handle.Init.NbrOfDiscConversion = 0;               /* 配置间断模式的规则通道个数，禁止规则通道组间断模式后，此参数忽略 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* 触发转换方式：软件触发 */
    HAL_ADC_Init(&g_adc_handle);                             /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc_handle);              /* 校准ADC */
}


/**
 * @brief       设置ADC通道采样时间
 * @param       adcx : adc句柄指针,ADC_HandleTypeDef
 * @param       ch   : 通道号, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       stime: 采样时间  0~7, 对应关系为:
 *   @arg       ADC_SAMPLETIME_1CYCLE_5, 1.5个ADC时钟周期        ADC_SAMPLETIME_7CYCLES_5, 7.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_13CYCLES_5, 13.5个ADC时钟周期     ADC_SAMPLETIME_28CYCLES_5, 28.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_41CYCLES_5, 41.5个ADC时钟周期     ADC_SAMPLETIME_55CYCLES_5, 55.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_71CYCLES_5, 71.5个ADC时钟周期     ADC_SAMPLETIME_239CYCLES_5, 239.5个ADC时钟周期
 * @param       rank: 多通道采集时需要设置的采集编号,
                假设你定义channle1的rank=1，channle2 的rank=2，
                那么对应你在DMA缓存空间的变量数组AdcDMA[0] 就i是channle1的转换结果，AdcDMA[1]就是通道2的转换结果。 
                单通道DMA设置为 ADC_REGULAR_RANK_1
 *   @arg       编号1~16：ADC_REGULAR_RANK_1~ADC_REGULAR_RANK_16
 * @retval      无
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime)
{
    ADC_ChannelConfTypeDef adc_ch_conf;
    
    adc_ch_conf.Channel = ch;                            /* 通道 */
    adc_ch_conf.Rank = rank;                             /* 序列 */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                    /* 采样时间 */
    HAL_ADC_ConfigChannel(adc_handle, &adc_ch_conf);     /* 通道配置 */
}

/**
 * @brief       获得ADC转换后的结果
 * @param       ch: 通道值 0~17，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_17
 * @retval      无
 */
uint32_t adc_get_result(uint32_t ch)
{
    adc_channel_set(&g_adc_handle , ch, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);    /* 设置通道，序列和采样时间 */

    HAL_ADC_Start(&g_adc_handle);                            /* 开启ADC */
    HAL_ADC_PollForConversion(&g_adc_handle, 10);            /* 轮询转换 */
    return (uint16_t)HAL_ADC_GetValue(&g_adc_handle);        /* 返回最近一次ADC1规则组的转换结果 */
}

/**
 * @brief       获取通道ch的转换值，取times次,然后平均
 * @param       ch      : 通道号, 0~17
 * @param       times   : 获取次数
 * @retval      通道ch的times次转换结果平均值
 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)     /* 获取times次数据 */
    {
        temp_val += adc_get_result(ch);
        //delay_us(1);
    }

    return temp_val / times;        /* 返回平均值 */
}








