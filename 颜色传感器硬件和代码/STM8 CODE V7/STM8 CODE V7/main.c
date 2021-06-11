/*---------------------��ɫ�������Ĳ��Գ���-------------------------*/
#include"iostm8s903k3.h"
#include"tcs34725.h"
#include"process.h"
#include"delay.h"

unsigned char color = 0;
u16 R, G, B, min, max,delta = 0;
double H, S, V = 0;

void CLK_init(void)
{
CLK_CKDIVR = 0x00; // 16M�ڲ�RC��1��Ƶ��ϵͳʱ��Ϊ16M
}

void LED_init(void)
{
  //R
  PD_DDR |= (1<<7); // ����PD7�˿ڵķ���Ĵ���PE5���
  PD_CR1 |= (1<<7); // ����PD7Ϊ������
  PD_CR2 &= ~(1<<7); /* �����ģʽ DDR=1 ʱ���ٶ� 2MHz *///���ģʽ
  //Y
  PE_DDR |= (1<<5); // ����PE5�˿ڵķ���Ĵ���PE5���
  PE_CR1 |= (1<<5); // ����PE5Ϊ������
  PE_CR2 &= ~(1<<5); /* �����ģʽ DDR=1 ʱ���ٶ� 2MHz *///���ģʽ
  //B
  PD_DDR |= (1<<3); // ����PD3�˿ڵķ���Ĵ���PE5���
  PD_CR1 |= (1<<3); // ����PD3Ϊ������
  PD_CR2 &= ~(1<<3); /* �����ģʽ DDR=1 ʱ���ٶ� 2MHz *///���ģʽ
  //W
  PC_DDR |= (1<<2); // ����PC2�˿ڵķ���Ĵ���PE5���
  PC_CR1 |= (1<<2); // ����PC2Ϊ������
  PC_CR2 &= ~(1<<2); /* �����ģʽ DDR=1 ʱ���ٶ� 2MHz *///���ģʽ
  //G
  PD_DDR |= (1<<4); // ����PD4�˿ڵķ���Ĵ���PE5���
  PD_CR1 |= (1<<4); // ����PD4Ϊ������
  PD_CR2 &= ~(1<<4); /* �����ģʽ DDR=1 ʱ���ٶ� 2MHz *///���ģʽ
  //O
  PC_DDR |= (1<<1); // ����PC1�˿ڵķ���Ĵ���PE5���
  PC_CR1 |= (1<<1); // ����PC1Ϊ������
  PC_CR2 &= ~(1<<1); /* �����ģʽ DDR=1 ʱ���ٶ� 2MHz *///���ģʽ
  IIC_SDA1_ON;
  LED_G_ON;
  LED_B_ON;
  LED_R_ON;
  IIC_SDA3_ON;
  IIC_SDA2_ON;
}

int main(void)
{
  CLK_init();
  LED_init();
  delay_init(16);
  uart1_init();  //57600bps
  asm("rim"); // ��ȫ���ж�
  TCS34725_Init_All();//��ʽʹ����Ҫ��
  delay_ms(10);
  while(1)
  {
    if((start_flag==1)&&(send_flag==0))
    {
      Get_Data_I2C();  //��ʽ�Ķ�ȡ10ͨ��
      start_flag = 0;
      delay_ms(10);
    }
    else if((start_flag==0)&&(send_flag==1))
    {
      Send_Data();
      send_flag = 0;
      //Reset_Send_Buff();
      LED_R_ON;
      LED_G_ON;
      LED_B_ON; 
      IIC_SDA1_ON; 
      IIC_SDA2_ON; 
      IIC_SDA3_ON;
    }
  }
}