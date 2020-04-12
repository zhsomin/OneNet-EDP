#ifndef _ONENET_H_
#define _ONENET_H_





_Bool OneNet_DevLink(void);

unsigned char OneNet_FillBuf(char *buf);

void OneNet_RevPro(unsigned char *cmd);
void OneNet_SendData(void);

_Bool OneNet_PushData(const char* dst_devid, const char* data, unsigned int data_len);
	
#endif
