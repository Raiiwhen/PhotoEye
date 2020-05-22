#define BSP_MODULE
#include <bsp.h>

#define BSP_REG_DEM_CR 							(*(CPU_REG32 *)0xE000EDFC) //DEMCR �Ĵ���
#define BSP_REG_DWT_CR 							(*(CPU_REG32 *)0xE0001000) //DWT ���ƼĴ���
#define BSP_REG_DWT_CYCCNT 						(*(CPU_REG32 *)0xE0001004) //DWT ʱ�Ӽ����Ĵ���
#define BSP_REG_DBGMCU_CR 						(*(CPU_REG32 *)0xE0042004)

//DEMCR�Ĵ����ĵ�24λ�����Ҫʹ��DWT ETM ITM��TPIU�Ļ�DEMCR�Ĵ����ĵ�24λ��1
#define BSP_BIT_DEM_CR_TRCENA 			DEF_BIT_24

//DWTCR �Ĵ����ĵ�0λ����Ϊ1��ʱ��ʹ��CYCCNT��������ʹ��CYCCCNT֮ǰӦ���ȳ�ʼ��
#define BSP_BIT_DWT_CR_CYCCNTENA 		DEF_BIT_00

//��ȡϵͳʱ��
//����ֵ��ϵͳʱ��HCLK
CPU_INT32U  BSP_CPU_ClkFreq (void)
{
	//����������ʹ�õ�HAL�⣬���Կ���ֱ�ӵ��ú��� ��ȡϵͳʱ��
	return HAL_RCC_GetHCLKFreq();	//����HCLKʱ��
}

//�˺������������ͳ�ʼ��CPU��ʱ�����ʱ������ʵ����ʹ��DWT��CYYCNT������
//��ʼ��CYCCNTΪ0
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
    CPU_INT32U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();

    BSP_REG_DEM_CR     |= (CPU_INT32U)BSP_BIT_DEM_CR_TRCENA;//ʹ��DWT
    BSP_REG_DWT_CYCCNT  = (CPU_INT32U)0u;		//��ʼ��CYCCNT�Ĵ���
    BSP_REG_DWT_CR     |= (CPU_INT32U)BSP_BIT_DWT_CR_CYCCNTENA;//����CYCCNT

    CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)fclk_freq);
}
#endif

//�˺������ڻ�ȡCYCCNT�е�ֵ
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    CPU_TS_TMR  ts_tmr_cnts;

    ts_tmr_cnts = (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;

    return (ts_tmr_cnts);
}
#endif

//��������������CPU_TS32_to_uSec()��CPU_TS64_to_uSec()������
//����ȡ����ʱ��������ת��Ϊus�ġ�
#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS32_to_uSec (CPU_TS32  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif


#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS64_to_uSec (CPU_TS64  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif
