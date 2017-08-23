/**
  ******************************************************************************
  * @file    udp_echoserver.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   UDP echo server
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
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#include "udp_echoserver.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define UDP_SERVER_PORT    7   /* define the UDP local connection port */
//#define UDP_CLIENT_PORT    7   /* define the UDP remote connection port */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void udp_echoserver_init(void)
{
   struct udp_pcb *upcb;
   err_t err;
   
   /* Create a new UDP control block  */
   upcb = udp_new();
   
   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
      
      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_echoserver_receive_callback, NULL);
      }
      else
      {
        udp_remove(upcb);
        printf("can not bind pcb");
      }
   }
   else
   {
     printf("can not create pcb");
   } 
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
extern uint8_t flag_udp_client;
u8_t	data[100];

void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
  /* Connect to the remote client */
  udp_connect(upcb, addr, UDP_SERVER_PORT);
    
	if (p != NULL)
	{
		if(flag_udp_client&(1<<3))//echo±êÖ¾Î»
		{
			/* copy data to pbuf */
			pbuf_take(p, (char*)data, strlen((char*)data));
				
			printf("Received Message: %s \r\n",p->payload);
			/* send udp data */
			udp_send(upcb, p); 
			/* free pbuf */
			pbuf_free(p);
		}else
		{
			/* copy data to pbuf */
			pbuf_take(p, (char*)data, strlen((char*)data));
				
			printf("Received Message: %s \r\n",p->payload);
			/* free pbuf */
			pbuf_free(p);
		}
	}
	else
	{
		/* free the UDP connection, so we can accept new clients */
		udp_remove(upcb);
		#ifdef SERIAL_DEBUG
		printf("\n\r can not allocate pbuf ");
		#endif
	}

  /* Free receive pbuf */
  pbuf_free(p);
  
//  /* free the UDP connection, so we can accept new clients */
//  udp_remove(upcb);     
}


uint8_t UdpsendBuf[200] = "Hello World!";
void udp_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	
	ptr=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)UdpsendBuf),PBUF_POOL);
	if(ptr!= NULL)
	{
		pbuf_take(ptr,(char*)UdpsendBuf,strlen((char*)UdpsendBuf));
		udp_send(upcb,ptr);	
		printf("Send Message: %s \r\n",UdpsendBuf);
		pbuf_free(ptr);
	}
} 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
