/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "main.h"
#include "tcp.h"
#include "udp.h"
#include "udp_echoserver.h"
#include "usart.h"
#include "led.h"
#include "key.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  10

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;
uint8_t flag_udp_client = 0;
extern uint8_t UdpsendBuf[200];
u8_t	UdprecvBuf[100];


/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	uint8_t KeyStatus = 0;

	struct udp_pcb *upcb;
	struct ip_addr DestIPaddr;
	struct pbuf *p;
	err_t err;
  /*!< At this stage the microcontroller clock setting is already configured to 
       168 MHz, this is done through SystemInit() function which is called from
       startup file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

#ifdef SERIAL_DEBUG
  USART1_init();
#endif

	printf("*****************************************");
  /*Initialize LCD and Leds */ 
  LED_Init();
  KEY_init();

  /* configure ethernet */ 
  ETH_BSP_Config();

  /* Initilaize the LwIP stack */
  LwIP_Init();

  /* Infinite loop */
  while (1)
  {  
		KeyStatus = Key_Scan(0);
		if(KeyStatus!=0)
		{
			switch(KeyStatus)
			{
				case 1://连接
					if((flag_udp_client&1<<1)==0)//连接管理标志位
					{
						flag_udp_client |=(1<<1);
						
						/* Create a new UDP control block  */
						upcb = udp_new();

						/* Connect to the remote client */
						if (upcb!=NULL)
						{
							/*assign destination IP address */
							IP4_ADDR( &DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );
						
							/* configure destination IP address and port */
							err= udp_connect(upcb, &DestIPaddr, UDP_SERVER_PORT);
							
							if (err == ERR_OK)
							{
								/* Set a receive callback for the upcb */
								udp_recv(upcb, udp_echoserver_receive_callback, NULL);

								sprintf((char*)UdprecvBuf, "New Connetion Established\r\n", (int*)NULL);
								printf("%s \r\n",UdprecvBuf);
								/* allocate pbuf from pool*/
								p = pbuf_alloc(PBUF_TRANSPORT,strlen((char*)UdprecvBuf), PBUF_POOL);
								if (p != NULL)
								{
									/* copy data to pbuf */
									pbuf_take(p, (char*)UdprecvBuf, strlen((char*)UdprecvBuf));

									/* send udp data */
									udp_send(upcb, p); 

									/* free pbuf */
									pbuf_free(p);
								}
								else
								{
									/* free the UDP connection, so we can accept new clients */
									udp_remove(upcb);
									#ifdef SERIAL_DEBUG
									printf("\n\r can not allocate pbuf ");
									#endif
								}
							}
							else
							{
								/* free the UDP connection, so we can accept new clients */
								udp_remove(upcb);
								#ifdef SERIAL_DEBUG
								 printf("\n\r can not connect udp pcb");
								#endif
							}
						}
					}
					break;
				case 2://发送
					{
						udp_senddata(upcb);
					}
					break;
				case 3://发送
					if((flag_udp_client&1<<3)==0)//echo标志位
					{
						flag_udp_client |=(1<<3);
						printf("Echo Manange: Start \r\n");
					}else
					{
						flag_udp_client &=~(1<<3);
						printf("Echo Manange: Stop\r\n");
					}
					break;
				default:;
			}
		}
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
