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
#include "string.h"
#include "lwip/tcp.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "udp_echoclient.h"

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

//uint8_t sendBuf[200] = "Hello World!";

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
  struct pbuf *p;
  struct ip_addr DestIPaddr;
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

	printf("**************************************************	\r\n");
  /*Initialize LCD and Leds */
  LED_Init();
	KEY_Init();

  /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
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
				case 1://����
					if((flag_udp_client&1<<1)==0)//���ӹ����־λ
					{
						/* Create a new UDP control block  */
						upcb = udp_new();
						
						if (upcb!=NULL)
						{
							/*assign destination IP address */
							IP4_ADDR( &DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );
						
							/* configure destination IP address and port */
							err= udp_connect(upcb, &DestIPaddr, UDP_SERVER_PORT);
							
							if (err == ERR_OK)
							{
								/* Set a receive callback for the upcb */
								udp_recv(upcb, udp_receive_callback, NULL);

//								sprintf((char*)recvBuf, "sending udp client message %d \r\n", (int*)message_count);
//								printf("%s \r\n",recvBuf);
								/* allocate pbuf from pool*/
								p = pbuf_alloc(PBUF_TRANSPORT,strlen((char*)recvBuf), PBUF_POOL);
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
						else
						{
							#ifdef SERIAL_DEBUG
							 printf("\n\r can not create udp pcb");
							#endif
						}
						flag_udp_client |=(1<<1);
					}
					break;
				case 2://����
					{
						/* copy data to pbuf */
						p=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)UdpsendBuf),PBUF_POOL); //�����ڴ�
						if(p)
						{
							pbuf_take(p,(char*)UdpsendBuf,strlen((char*)UdpsendBuf)); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
							printf("Send Message: %s \r\n",UdpsendBuf);
							udp_send(upcb,p);	//udp�������� 
							pbuf_free(p);//�ͷ��ڴ�
						}
					}
					break;
				case 3://����
					if((flag_udp_client&1<<3)==0)//echo��־λ
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

