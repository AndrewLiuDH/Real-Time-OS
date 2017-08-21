#include "stm32f4xx.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "lan8720a.h"
#include "netconf.h"
#include "stm32f4x7_eth.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  10

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;

/* Private function prototypes -----------------------------------------------*/
void System_Periodic_Handle(void);

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
  /* Capture the current local time */
  timingdelay = LocalTime + nCount;  

  /* wait until the desired delay finish */  
  while(timingdelay > LocalTime)
  {
  }
}

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}

int main()
{
	RCC_ClocksTypeDef RCC_Clocks;
	
	/* Configure Systick clock source as HCLK */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

	/* SystTick configuration: an interrupt every 10ms */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 100);

	LED_Init();
	USART1_init();
	printf("Init OK \r\n");

	ETH_BSP_Config();
	LwIP_Init();
	
	while(1)
	{
		/* check if any packet received */
		if (ETH_CheckFrameReceived())
		{ 
		  /* process received ethernet packet */
		  LwIP_Pkt_Handle();
		}
		/* handle periodic timers for LwIP */
		LwIP_Periodic_Handle(LocalTime);
	}
}
