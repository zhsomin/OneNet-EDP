#ifndef __LED_H
#define __LED_H	 
#include "sys.h"


#define LED1_ON    GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define LED1_OFF   GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define LED1      PBout(5)


#define LED2_ON    GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define LED2_OFF   GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define LED2      PBout(5)

#define LED3_ON    GPIO_ResetBits(GPIOB,GPIO_Pin_0)
#define LED3_OFF   GPIO_SetBits(GPIOB,GPIO_Pin_0)
#define LED3      PBout(0)



void LED_Init(void);//≥ı ºªØ
		 				    
#endif
