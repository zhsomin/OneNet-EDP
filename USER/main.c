
/********************************************************/
/*
*   功能：  基于STM32单片机与OneNet平台的LED灯控制实验  	
*   日期：  2018.05.18                                 
*   来源：  广东海洋大学																
*   作者：  智能控制团队Allen                           
*   PS：由于某些代码是自己敲的 可能不规范 或者有些瑕疵 大家凑合着看吧  
*  说明：该程序存在一个BUG 配置完AT指令后 延迟函数delay_ms 就有点失灵了大概延迟时间会缩小10倍 我找不到原因。
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

/*   全局变量定义  */
unsigned int Heart_Pack=0;  //用于定时器TIM1自加计数，用于满足设定自加数值时发送EDP心跳包的标志位
u8  AT_Mode=0;     //发AT指令的模式 用于表示程序处于配置Wifi模块的模式  对串口接收数据进行不同的处理
u8  Contral_flag=0;  //用于存贮控制指令  判断进行什么操作
uint8_t LED1STATUS=0 ;
uint8_t BeepSTATUS=0;
uint8_t LED3STATUS=0; 

void System_Init()
{
/*  基本功能初始化   */
	delay_init();		//延时初始化

	delay_ms(1000);		//等待系统模块启动
	
	uart_init(115200);    //打开串口1，与Wifi模块通信
	LED_Init();           //LED灯初始化
  BEEP_GPIO_Config();
/*  配置wifi模块     */
	ESP8266_GPIO_Init();  //初始化ESP8266的CH_DP引脚，拉高即可
	ESP8266Mode_inti();   //初始化ESP8266的AT指令

/*  打开定时器       */
	Tim1_Init(5000,72);   //定时器周期配置为5MS
}

void Connect_OneNet(void)
{
	while(!OneNet_Connect)     //判断是否接入平台成功
	{	
		while(OneNet_DevLink())			//接入OneNET
		delay_ms(1000);       
	}
}


int main(void)
{	 

	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};   //定义一个结构体变量   供心跳包装载数据用
		
    System_Init();       //初始化各模块
		
	Connect_OneNet();    //接入OneNet平台

	while(1)	             //while循环执行各种功能 
	{  
  /*  定时发送心跳包  */			
		if(Heart_Pack>=5000)              //每隔25秒发一次心跳包   OneNet平台默认四分钟无数据交换就会断开与设备的连接 发心跳包可以保证连接。
		{
			EDP_PacketPing(&edpPacket);     //装载并发送心跳包
			Heart_Pack=0;
		}    
		 
	/*用strstr函数来判断操作指令是否匹配 对该函数不了解的朋友自行百度吧*/	 
		if(strstr((const char*)Message_Buf,"OPEN"))    //判断到操作指令为OPEN 
		{
			LED2_ON;   //打开LED
			LED1STATUS=1;
			OneNet_SendData();  //向平台发数据流test的数值为1
			delay_ms(20);     //延迟
			memset(Message_Buf, 0, sizeof(Message_Buf));    //执行完指令 清空指令存贮空间 防止继续执行该指令
		}
		if(strstr((const char*)Message_Buf,"CLOSE"))   //判断到操作指令为CLOSE 
		{
			LED2_OFF; //关闭LED
			LED1STATUS=0;
			OneNet_SendData();  //向平台发数据流test的数值为0
			delay_ms(20);   //延迟
			memset(Message_Buf, 0, sizeof(Message_Buf));    //执行完指令 清空指令存贮空间 防止继续执行该指令
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
	
	if(strstr((const char*)Message_Buf,"LED21"))    //判断到操作指令为OPEN 
		{
			 LED3_ON;   //打开LED
			 LED3STATUS=1;
			OneNet_SendData();  //向平台发数据流test的数值为1
			delay_ms(20);     //延迟
			memset(Message_Buf, 0, sizeof(Message_Buf));    //执行完指令 清空指令存贮空间 防止继续执行该指令
		}
		if(strstr((const char*)Message_Buf,"LED20"))   //判断到操作指令为CLOSE 
		{
			LED3_OFF; //关闭LED
			LED3STATUS=0;
			OneNet_SendData();  //向平台发数据流test的数值为0
			delay_ms(20);   //延迟
			memset(Message_Buf, 0, sizeof(Message_Buf));    //执行完指令 清空指令存贮空间 防止继续执行该指令
		}




		
		
	}	
}


		
