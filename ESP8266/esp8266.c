/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	esp8266.c
	*
	*	ԭ���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		ESP8266�ļ�����
	*
	*	�汾�޸ļ�¼��	��
	
	* �޸��ߣ� �㶫�����ѧ���ܿ����Ŷ� Allen
	
	* ����  �� 2018.05.18
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "esp8266.h"

//Ӳ������
#include "delay.h"
#include "usart.h"
#include "Led.h"
//C��
#include <string.h>
#include <stdio.h>



/**
  * @brief  ��ʼ��ESP8266�õ���GPIO����
  * @param  ��
  * @retval ��
  */
void ESP8266_GPIO_Init(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;


	/* ���� CH_PD ����*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;               //�����������ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;                      //��ʼ��CH-DP���� ����
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;              //����ʱ��
	GPIO_Init(GPIOB,&GPIO_InitStructure);   
	GPIO_SetBits(GPIOB,GPIO_Pin_8); 

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;               //�����������ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;                      //��ʼ��RST���� ����
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;              //����ʱ��
	GPIO_Init(GPIOB,&GPIO_InitStructure);   
	GPIO_SetBits(GPIOB,GPIO_Pin_9); 
  
}

void ESP8266Mode_inti(void)
{
	int Delay = 1000;
	
	unsigned char Cmd[]="+++";//�˳�͸��ģʽָ��
	
	AT_Mode = 1;  //���뷢ATָ��ģʽ

	/****wifiģ����OneNetƽ̨���ݶԽ����� */
	
	USART_Write(Cmd,strlen((const char*)Cmd));//�����˳�͸��ģʽ,������ATָ��
	delay_ms(100);				//�ȴ��˳�͸��
	
	SendCmd(AT,Delay);			//ģ����Ч�Լ��
	SendCmd(CWMODE,Delay);		//ģ�鹤��ģʽ
	SendCmd(wifi_RST,Delay);	//ģ������

	delay_ms(1000);				//�ȴ�ģ�鸴λ���

	SendCmd(CWJAP,1500);		//������Ҫ���ӵ�WIFI�ȵ�SSID������  CIFSR
	SendCmd(CIPSTART,Delay);	//TCP����
	SendCmd(CIPMODE1,Delay);	//����͸��ģʽ
	SendCmd(CIPSEND,Delay);		//����͸��ģʽ

	AT_Mode = 0;				//ATָ������ �˳���ģʽ
}


	
/** 
	* �������ܣ� ����Cmd����ĺ���
  * CMD�� 		��Ҫ���͵�ATָ��
  * result �� ���ͳɹ�ʱ���ص���ֵ��result��������Ա�
  * timeOut ���ӳ�ʱ��
  *	
*/	
	
void SendCmd(char* cmd, int timeOut)
{
    while(1)
    {
		memset(Rx_Buff, 0, sizeof(Rx_Buff)); //��������֮ǰ��������������飬�����ڴ����н��ա�
		Rx_count=0;

		USART_Write((unsigned char *)cmd,strlen((const char *)cmd));   //�ô��ڰ�cmd����д��ESP8266

		delay_ms(timeOut);                                          //�ӳٵȴ�
		LED1=~LED1;        //��������

		if(ok_flag==1)	//�����жϺ����У������ж��Ƿ񷵻�"OK"��">"��������������ok_flag����1��������ǰATָ������ã���ʼ������һ��
		{  
			ok_flag=0;   //��ձ�־
			break;
		}
		else
		{
			delay_ms(100);		
		}
    }
}


/**   
 * �������ܣ�ESP8266д���ݵĺ���

 *   

*/
void ESP8266_SendData(unsigned char *data, unsigned short len)
{
  	USART_Write(data,len);   //��������
}


