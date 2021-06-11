/*---------------------颜色传感器的测试程序-------------------------*/
#include"iostm8s903k3.h"
#include"tcs34725.h"
#include"process.h"
#include"delay.h"

unsigned char color = 0;
u16 R, G, B, min, max,delta = 0;
double H, S, V = 0;

void CLK_init(void)
{
CLK_CKDIVR = 0x00; // 16M内部RC经1分频后系统时钟为16M
}

void LED_init(void)
{
  //R
  PD_DDR |= (1<<7); // 配置PD7端口的方向寄存器PE5输出
  PD_CR1 |= (1<<7); // 设置PD7为推挽输
  PD_CR2 &= ~(1<<7); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
  //Y
  PE_DDR |= (1<<5); // 配置PE5端口的方向寄存器PE5输出
  PE_CR1 |= (1<<5); // 设置PE5为推挽输
  PE_CR2 &= ~(1<<5); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
  //B
  PD_DDR |= (1<<3); // 配置PD3端口的方向寄存器PE5输出
  PD_CR1 |= (1<<3); // 设置PD3为推挽输
  PD_CR2 &= ~(1<<3); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
  //W
  PC_DDR |= (1<<2); // 配置PC2端口的方向寄存器PE5输出
  PC_CR1 |= (1<<2); // 设置PC2为推挽输
  PC_CR2 &= ~(1<<2); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
  //G
  PD_DDR |= (1<<4); // 配置PD4端口的方向寄存器PE5输出
  PD_CR1 |= (1<<4); // 设置PD4为推挽输
  PD_CR2 &= ~(1<<4); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
  //O
  PC_DDR |= (1<<1); // 配置PC1端口的方向寄存器PE5输出
  PC_CR1 |= (1<<1); // 设置PC1为推挽输
  PC_CR2 &= ~(1<<1); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
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
  asm("rim"); // 开全局中断
  TCS34725_Init_All();//正式使用需要改
  delay_ms(10);
  while(1)
  {
    if((start_flag==1)&&(send_flag==0))
    {
      Get_Data_I2C();  //正式的读取10通道
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