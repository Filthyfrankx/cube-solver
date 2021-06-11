#ifndef _PROCESS_H
#define _PROCESS_H

#define u8 unsigned char
#define u16 unsigned int
#define LED_R_ON PD_ODR |= (1<<7)              //指示灯亮
#define LED_G_ON PD_ODR |= (1<<4)
#define LED_B_ON PD_ODR |= (1<<3)
#define IIC_SDA1_ON PE_ODR |= (1<<5)
#define IIC_SDA2_ON PC_ODR |= (1<<1)
#define IIC_SDA3_ON PC_ODR |= (1<<2)

#define LED_R_OFF PD_ODR &= ~(1<<7)            //指示灯灭
#define LED_G_OFF PD_ODR &= ~(1<<4)
#define LED_B_OFF PD_ODR &= ~(1<<3)
#define IIC_SDA1_OFF PE_ODR &= ~(1<<5)
#define IIC_SDA2_OFF PC_ODR &= ~(1<<1)
#define IIC_SDA3_OFF PC_ODR &= ~(1<<2)
extern u8 start_flag;
extern u8 send_flag;

char Get_Colour(int H,int S,int V);//Get_colour
char Min3v(int x,int y,int z);
char Max3v(int x,int y,int z);
void UART1_SendChar(unsigned char ch);
void uart1_init(void);
void Get_Data_I2C(void);
void Send_Data(void);
void Reset_Send_Buff();
#endif
