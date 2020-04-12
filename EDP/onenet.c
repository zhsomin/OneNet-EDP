/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�汾�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	
	*
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "edpkit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"
#include "led.h"

//C��
#include <string.h>
#include <stdio.h>


#define DEVID	"589181049"

#define APIKEY	"QC5uXz=N3eMiGHGU4iI24ptpLwo="

//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================


extern uint8_t LED1STATUS;
extern uint8_t BeepSTATUS;
extern uint8_t LED3STATUS;

_Bool OneNet_DevLink(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//Э���
	
	unsigned char status = 1;
	

	if(EDP_PacketConnect1(DEVID, APIKEY, 256, &edpPacket) == 0)		//����devid �� apikey��װЭ���
	{
		
		ESP8266_SendData(edpPacket._data, edpPacket._len);			//�ϴ�ƽ̨
	
		delay_ms(500);      //�ӳٵȴ�ʱ�����300ms һ��Ҫ�� ��Ȼ������
		EDP_DeleteBuffer(&edpPacket);								//ɾ��
		status=0;
	}
	
	return status;
	
}



unsigned char OneNet_FillBuf(char *buf)
{
	
	char text[250];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{");


    //�ƹ����
    memset(text, 0, sizeof(text));
	  sprintf(text, "\"test\":%d,", LED1STATUS);
	  strcat(buf, text);
	
	  
	  memset(text, 0, sizeof(text));
	  sprintf(text, "\"led2\":%d,", LED3STATUS);    
	  strcat(buf, text);
     	
	
    
    memset(text, 0, sizeof(text));
	  sprintf(text, "\"Beep\":%d", BeepSTATUS);
	  strcat(buf, text);

	
 
	strcat(buf, "}");

	return strlen(buf);

}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����DataStream ���������֣� data ��ֵ		
//==========================================================
void OneNet_SendData()
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[250];
	
	short body_len = 0, i = 0;

	memset(buf, 0, sizeof(buf));
	memset(Rx_Buff, 0, sizeof(Rx_Buff));
	Rx_count=0;
	body_len = OneNet_FillBuf(buf);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(EDP_PacketSaveData(DEVID, body_len, NULL, kTypeSimpleJsonWithoutTime, &edpPacket) == 0)	//���
		{
			for(; i < body_len; i++)
				edpPacket._data[edpPacket._len++] = buf[i];
			
			ESP8266_SendData(edpPacket._data, edpPacket._len);										//�ϴ����ݵ�ƽ̨
			
			EDP_DeleteBuffer(&edpPacket);															//ɾ��	
		}
	}
	
}
