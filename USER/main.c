
/********************************************************/
/*
*   ���ܣ�  ����STM32��Ƭ����OneNetƽ̨��LED�ƿ���ʵ��  	
*   ���ڣ�  2018.05.18                                 
*   ��Դ��  �㶫�����ѧ																
*   ���ߣ�  ���ܿ����Ŷ�Allen                           
*   PS������ĳЩ�������Լ��õ� ���ܲ��淶 ������Щ覴� ��Ҵպ��ſ���  
*  ˵�����ó������һ��BUG ������ATָ��� �ӳٺ���delay_ms ���е�ʧ���˴���ӳ�ʱ�����С10�� ���Ҳ���ԭ��
*/
/********************************************************/
#include "stm32f10x.h"
#include "esp8266.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"
#include "sys.h"
#include "led.h"
#include "onenet.h"
#include "string.h" 
#include "Tim.h"
#include "bsp_beep.h"   

/*   ȫ�ֱ�������  */
unsigned int Heart_Pack=0;  //���ڶ�ʱ��TIM1�ԼӼ��������������趨�Լ���ֵʱ����EDP�������ı�־λ
u8  AT_Mode=0;     //��ATָ���ģʽ ���ڱ�ʾ����������Wifiģ���ģʽ  �Դ��ڽ������ݽ��в�ͬ�Ĵ���
u8  Contral_flag=0;  //���ڴ�������ָ��  �жϽ���ʲô����
uint8_t LED1STATUS=0 ;
uint8_t BeepSTATUS=0;
uint8_t LED3STATUS=0; 

void System_Init()
{
/*  �������ܳ�ʼ��   */
	delay_init();		//��ʱ��ʼ��

	delay_ms(1000);		//�ȴ�ϵͳģ������
	
	uart_init(115200);    //�򿪴���1����Wifiģ��ͨ��
	LED_Init();           //LED�Ƴ�ʼ��
  BEEP_GPIO_Config();
/*  ����wifiģ��     */
	ESP8266_GPIO_Init();  //��ʼ��ESP8266��CH_DP���ţ����߼���
	ESP8266Mode_inti();   //��ʼ��ESP8266��ATָ��

/*  �򿪶�ʱ��       */
	Tim1_Init(5000,72);   //��ʱ����������Ϊ5MS
}

void Connect_OneNet(void)
{
	while(!OneNet_Connect)     //�ж��Ƿ����ƽ̨�ɹ�
	{	
		while(OneNet_DevLink())			//����OneNET
		delay_ms(1000);       
	}
}


int main(void)
{	 

	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};   //����һ���ṹ�����   ��������װ��������
		
    System_Init();       //��ʼ����ģ��
		
	Connect_OneNet();    //����OneNetƽ̨

	while(1)	             //whileѭ��ִ�и��ֹ��� 
	{  
  /*  ��ʱ����������  */			
		if(Heart_Pack>=5000)              //ÿ��25�뷢һ��������   OneNetƽ̨Ĭ���ķ��������ݽ����ͻ�Ͽ����豸������ �����������Ա�֤���ӡ�
		{
			EDP_PacketPing(&edpPacket);     //װ�ز�����������
			Heart_Pack=0;
		}    
		 
	/*��strstr�������жϲ���ָ���Ƿ�ƥ�� �Ըú������˽���������аٶȰ�*/	 
		if(strstr((const char*)Message_Buf,"OPEN"))    //�жϵ�����ָ��ΪOPEN 
		{
			LED2_ON;   //��LED
			LED1STATUS=1;
			OneNet_SendData();  //��ƽ̨��������test����ֵΪ1
			delay_ms(20);     //�ӳ�
			memset(Message_Buf, 0, sizeof(Message_Buf));    //ִ����ָ�� ���ָ������ռ� ��ֹ����ִ�и�ָ��
		}
		if(strstr((const char*)Message_Buf,"CLOSE"))   //�жϵ�����ָ��ΪCLOSE 
		{
			LED2_OFF; //�ر�LED
			LED1STATUS=0;
			OneNet_SendData();  //��ƽ̨��������test����ֵΪ0
			delay_ms(20);   //�ӳ�
			memset(Message_Buf, 0, sizeof(Message_Buf));    //ִ����ָ�� ���ָ������ռ� ��ֹ����ִ�и�ָ��
		}
//////////////////////////////////////////////////////////////////////////
	


		if(strstr((const char*)Message_Buf,"Beep1"))
		{
			BEEP(ON);
			BeepSTATUS=1;
			OneNet_SendData();
			delay_ms(20);
			memset(Message_Buf, 0, sizeof(Message_Buf));
		}
		if(strstr((const char*)Message_Buf,"Beep0"))
		{
			BEEP(OFF);
			BeepSTATUS=0;
			OneNet_SendData();
			delay_ms(20);
			memset(Message_Buf, 0, sizeof(Message_Buf));
		}
	
	if(strstr((const char*)Message_Buf,"LED21"))    //�жϵ�����ָ��ΪOPEN 
		{
			 LED3_ON;   //��LED
			 LED3STATUS=1;
			OneNet_SendData();  //��ƽ̨��������test����ֵΪ1
			delay_ms(20);     //�ӳ�
			memset(Message_Buf, 0, sizeof(Message_Buf));    //ִ����ָ�� ���ָ������ռ� ��ֹ����ִ�и�ָ��
		}
		if(strstr((const char*)Message_Buf,"LED20"))   //�жϵ�����ָ��ΪCLOSE 
		{
			LED3_OFF; //�ر�LED
			LED3STATUS=0;
			OneNet_SendData();  //��ƽ̨��������test����ֵΪ0
			delay_ms(20);   //�ӳ�
			memset(Message_Buf, 0, sizeof(Message_Buf));    //ִ����ָ�� ���ָ������ռ� ��ֹ����ִ�и�ָ��
		}




		
		
	}	
}


		
