#include "sys.h"
#include "usart.h"	
#include "delay.h"
#include "string.h" 	 
#include "esp8266.h"
#include "led.h"


unsigned char Message_length;

int OneNet_Connect=0;      //�����ж�wifiģ���Ƿ�������OneNetƽ̨�ı�־

char Rx_Buff[200];
int  Rx_count=0;   //����ESP8266�жϽ������ݵĶ���
int  ok_flag=0;
char Message_Buf[20];  //���ڴ�������ָ��
u16 USART_RX_STA=0;       //����״̬���	  

#define WIFI_UART	USART1	//�궨�崮��

//����Ҫ�޸Ĵ��ڣ���Ҫ�����²������
/*
 1.�޸�uart_init�����еĴ������ã�����ʱ�ӡ����š��ж����õȡ�
 2.�޸ĺ궨�� WIFI_UART �������ֵ,��ǰĬ��ʹ�ô���1 USART1
 3.�޸Ĵ����ж���ں���
*/


/** ���ڷ���һ�����ݵĺ���  */
void USART_Write(unsigned char *cmd, int len)
{  
	int i;
	
	USART_ClearFlag(WIFI_UART,USART_FLAG_TC);    //����֮ǰ��շ��ͱ�־  û����һ�� �����׶��� ��һ���������׶�ʧ
	
	for(i=0;i<len;i++)
	{
		USART_SendData(WIFI_UART,*cmd);   //���͵�ǰ����
		while(USART_GetFlagStatus(WIFI_UART,USART_FLAG_TC)!= SET);  //�������
		cmd++;      //ָ���Լ� ׼��������һ������
	}
 
}

//��ʼ��IO ����1
void uart_init(u32 bound)
{
		//GPIO�˿�����
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);	//ʹ��USART1��GPIOAʱ��

		USART_DeInit(WIFI_UART);  //��λ����1
		//UART1_TX   PA.9
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA9
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
		GPIO_Init(GPIOA, &GPIO_InitStructure); //

		//UART1_RX	  PA.10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
		GPIO_Init(GPIOA, &GPIO_InitStructure);  //

		//UART1 NVIC ����

		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

		//USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

		USART_Init(WIFI_UART, &USART_InitStructure); //��ʼ������
		USART_ITConfig(WIFI_UART, USART_IT_RXNE, ENABLE);//�����ж�
		USART_Cmd(WIFI_UART, ENABLE);                    //ʹ�ܴ��� 

}


void Receive_AT_Data()
{    //�ж�ESP8266�Ƿ񷵻�'OK'��'>'��'ready' 
		if(strstr(Rx_Buff,"OK")||strstr(Rx_Buff,">")||strstr(Rx_Buff,"ready"))    
		{
			 memset(Rx_Buff, 0, sizeof(Rx_Buff));  //���Rx_Buff����
			 ok_flag=1;			                //�жϳɹ�����־λ��1
			 Rx_count=0;				
		}
		else                                   //���������������
		{
			Rx_count++;	
			if(Rx_count>=150)                   //������150�����ݣ�������� �������
			{
				Rx_count=0;
				memset(Rx_Buff, 0, sizeof(Rx_Buff));
			}
		}
}

void Receive_Normal_Data()
{
	  static int i;        //��̬����i
	
		if(Rx_count==150)                   //������150�����ݣ�������� �������
		{
			Rx_count=0;                 	
			memset(Rx_Buff, 0, sizeof(Rx_Buff));  //��յ�ǰ������������
		}

		if(Message_length>0)                   //��ʾ���Կ�ʼ��������ָ��
		{
			Message_Buf[i]=Rx_Buff[Rx_count];   //��������ָ������
			i++;
			Message_length--;                  //��һ��ָ�ʣ��������һ,�жϲ���ָ���Ƿ�������
		}
		 
		if(Rx_count>3&&Rx_Buff[Rx_count-2]==0&&Rx_Buff[Rx_count-1]==0&&Rx_Buff[Rx_count]>0)   
		//�����ǰ���յ������ݴ���0������������ݵ�ǰ��������Ϊ00 ����ǰ���ݾ��ǲ���ָ��ĳ��ȡ�
		{
			 memset(Message_Buf, 0, sizeof(Message_Buf)); //��մ�������ָ������飬׼�������µĲ���ָ��
			
			 Message_length=Rx_Buff[Rx_count];      //�����յ������ݴ�Ϊ����ָ��ȡ�
			 i=0;                                   //���i
		}
		Rx_count++;                               //׼���洢��һ������ 
						
}


void USART1_IRQHandler(void)                	//����1�жϷ������
{
		u8 Res;	

		if(USART_GetITStatus(WIFI_UART, USART_IT_RXNE) != RESET)  //�����ж�
		{
			
			Res =USART_ReceiveData(WIFI_UART);	//��ȡ���յ�������			
			
			Rx_Buff[Rx_count]=Res;
			
	  	/*  ����ATָ��ģʽ�� ���ݵĴ��� */
			if(AT_Mode==1)//����ATָ���� ���ݵĴ���
			{
				Receive_AT_Data();  //����ATָ��ģʽ�µ�����
			}

		/*  ����ƽ̨�� ���ݵĴ��� */
			else if(AT_Mode==0)    //����ģʽ��������
			{			 	
				Receive_Normal_Data();  //����ģʽ�´�������
			}

			USART_ClearITPendingBit(WIFI_UART,USART_IT_RXNE);
		}	 


		/**********�ж�WiFiģ���Ƿ�������OneNet ƽ̨*******/
		{
			if(Rx_Buff[Rx_count-3]==0x20&&Rx_Buff[Rx_count-2]==0x02&&Rx_Buff[Rx_count-1]==0x00&&Rx_Buff[Rx_count]==0x00)  //������ƽ̨ʱ ƽ̨�᷵�� 20 02 00 00������
			{
				OneNet_Connect=1;         //������ɵı�־λ
			}
			if(Rx_Buff[Rx_count-2]==0x64&&Rx_Buff[Rx_count-1]==0x01&&Rx_Buff[Rx_count]==0x60)  //������ƽ̨ʱ ƽ̨�᷵�� 20 02 00 00������
			{
				OneNet_Connect=0;         //���ӶϿ�
			}
		}
} 

