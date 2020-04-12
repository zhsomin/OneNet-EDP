/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	esp8266.c
	*
	*	原作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.0
	*
	*	说明： 		ESP8266的简单驱动
	*
	*	版本修改记录：	无
	
	* 修改者： 广东海洋大学智能控制团队 Allen
	
	* 日期  ： 2018.05.18
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备驱动
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "usart.h"
#include "Led.h"
//C库
#include <string.h>
#include <stdio.h>



/**
  * @brief  初始化ESP8266用到的GPIO引脚
  * @param  无
  * @retval 无
  */
void ESP8266_GPIO_Init(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;


	/* 配置 CH_PD 引脚*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;               //设置推挽输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;                      //初始化CH-DP引脚 拉高
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;              //设置时钟
	GPIO_Init(GPIOB,&GPIO_InitStructure);   
	GPIO_SetBits(GPIOB,GPIO_Pin_8); 

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;               //设置推挽输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;                      //初始化RST引脚 拉高
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;              //设置时钟
	GPIO_Init(GPIOB,&GPIO_InitStructure);   
	GPIO_SetBits(GPIOB,GPIO_Pin_9); 
  
}

void ESP8266Mode_inti(void)
{
	int Delay = 1000;
	
	unsigned char Cmd[]="+++";//退出透传模式指令
	
	AT_Mode = 1;  //进入发AT指令模式

	/****wifi模块与OneNet平台数据对接配置 */
	
	USART_Write(Cmd,strlen((const char*)Cmd));//先行退出透传模式,再配置AT指令
	delay_ms(100);				//等待退出透传
	
	SendCmd(AT,Delay);			//模块有效性检查
	SendCmd(CWMODE,Delay);		//模块工作模式
	SendCmd(wifi_RST,Delay);	//模块重置

	delay_ms(1000);				//等待模块复位完成

	SendCmd(CWJAP,1500);		//配置需要连接的WIFI热点SSID和密码  CIFSR
	SendCmd(CIPSTART,Delay);	//TCP连接
	SendCmd(CIPMODE1,Delay);	//配置透传模式
	SendCmd(CIPSEND,Delay);		//进入透传模式

	AT_Mode = 0;				//AT指令发送完毕 退出该模式
}


	
/** 
	* 函数功能： 发送Cmd命令的函数
  * CMD： 		需要发送的AT指令
  * result ： 发送成功时返回的数值与result期望结果对比
  * timeOut ：延迟时间
  *	
*/	
	
void SendCmd(char* cmd, int timeOut)
{
    while(1)
    {
		memset(Rx_Buff, 0, sizeof(Rx_Buff)); //发送数据之前，先情况接收数组，数据在串口中接收。
		Rx_count=0;

		USART_Write((unsigned char *)cmd,strlen((const char *)cmd));   //用串口把cmd命令写给ESP8266

		delay_ms(timeOut);                                          //延迟等待
		LED1=~LED1;        //闪灯提醒

		if(ok_flag==1)	//串口中断函数中，自行判断是否返回"OK"或">"，若符合条件，ok_flag则置1，跳出当前AT指令的配置，开始配置下一条
		{  
			ok_flag=0;   //清空标志
			break;
		}
		else
		{
			delay_ms(100);		
		}
    }
}


/**   
 * 函数功能：ESP8266写数据的函数

 *   

*/
void ESP8266_SendData(unsigned char *data, unsigned short len)
{
  	USART_Write(data,len);   //发送数据
}


