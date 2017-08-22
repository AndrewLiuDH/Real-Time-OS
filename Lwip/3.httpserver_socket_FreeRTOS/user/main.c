/* Includes ------------------------------------------------------------------*/
#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "main.h"
#include "usart.h"
#include "tcpip.h"
#include "httpserver-socket.h"
#include "led.h"

/* Private typedef -----------------------------------------------------------*/
void vApplicationIdleHook(void)
{
//  printf("StackOverflow \r\n");
}
void vApplicationTickHook(void)
{
#if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 0 )
    {
        // In this case the tick hook is used as part of the queue set test. 
    }
#endif // mainCREATE_SIMPLE_BLINKY_DEMO_ONLY 
}
void vApplicationMallocFailedHook(void)
{
	printf("MallocFailed \r\n");
	taskDISABLE_INTERRUPTS();
	for(;;);
}

void vApplicationStackOverflowHook(void)
{
	printf("StackOverflow \r\n");
}/* Private define ------------------------------------------------------------*/
/*--------------- LCD Messages ---------------*/
#define MESSAGE1   "    STM32F40/41x    "
#define MESSAGE2   "  STM32F-4 Series   "
#define MESSAGE3   " UDP/TCP EchoServer "
#define MESSAGE4   "                    "

/*--------------- Tasks Priority -------------*/
#define MAIN_TASK_PRIO   ( tskIDLE_PRIORITY + 1 )
#define DHCP_TASK_PRIO   ( tskIDLE_PRIORITY + 4 )
#define LED_TASK_PRIO    ( tskIDLE_PRIORITY + 2 )
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern struct netif xnetif;
__IO uint32_t test;
 
/* Private function prototypes -----------------------------------------------*/
void LCD_LED_Init(void);
void Main_task(void * pvParameters);
void ToggleLed4(void * pvParameters);
extern void tcpecho_init(void);
extern void udpecho_init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured to 
       168 MHz, this is done through SystemInit() function which is called from
       startup file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */

 /* Configures the priority grouping: 4 bits pre-emption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  /* Init task */
  xTaskCreate(Main_task,
			(const char *)"Main",
			configMINIMAL_STACK_SIZE * 2,
			NULL,MAIN_TASK_PRIO,
			NULL);
  
  /* Start scheduler */
  vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler */
  for( ;; );
}

/**
  * @brief  Main task
  * @param  pvParameters not used
  * @retval None
  */
void Main_task(void * pvParameters)
{
#ifdef SERIAL_DEBUG
  USART1_init();
#endif
	printf("**********************************************\r\n");
	/*Initialize LCD and Leds */ 
	LCD_LED_Init();

	/* configure ethernet (GPIOs, clocks, MAC, DMA) */ 
	ETH_BSP_Config();

	/* Initilaize the LwIP stack */
	LwIP_Init();

  /* Initialize webserver demo */
  http_server_socket_init();

	#ifdef USE_DHCP
	/* Start DHCPClient */
	xTaskCreate(LwIP_DHCP_task, 
			  (const char *)"DHCP", 
			  configMINIMAL_STACK_SIZE * 2,
			  NULL,DHCP_TASK_PRIO,
			  NULL);
#endif

  /* Start toogleLed4 task : Toggle LED4  every 250ms */
  xTaskCreate(ToggleLed4,
			  (const char *)"LED4",
			  configMINIMAL_STACK_SIZE,
			  NULL, LED_TASK_PRIO,
			  NULL);
  for( ;; )
  {
    vTaskDelete(NULL);
  }
}

/**
  * @brief  Toggle Led4 task
  * @param  pvParameters not used
  * @retval None
  */
void ToggleLed4(void * pvParameters)
{
  while(1)
  {   
    test = xnetif.ip_addr.addr;
    /*check if IP address assigned*/
    if (test !=0)
    {
      for( ;; )
      {
        /* toggle LED4 each 250ms */
        LED4=!LED4;
        vTaskDelay(250);
      }
    }
  }
}

/**
  * @brief  Initializes the STM324xG-EVAL's LCD and LEDs resources.
  * @param  None
  * @retval None
  */
void LCD_LED_Init(void)
{
  LED_Init();
  
#ifdef SERIAL_DEBUG

  /* Display message on the LCD*/
	printf("MESSAGE1: %s\r\n",MESSAGE1);
	printf("MESSAGE2: %s\r\n",MESSAGE2);
	printf("MESSAGE3: %s\r\n",MESSAGE3);
	printf("MESSAGE4: %s\r\n",MESSAGE4);  
#endif
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of Ticks to delay.
  * @retval None
  */
void Delay(uint32_t nCount)
{
  vTaskDelay(nCount);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
