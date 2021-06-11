#include"delay.h"
volatile unsigned int fac_us=0;

void delay_init(unsigned int clk)
{
  if(clk>16)
    fac_us=(16-4)/4;
  else if(clk>4)
    fac_us=(clk-4)/4; 
  else fac_us=1;
}

/*
//����ʱ����������debug
void delay_us(unsigned int nus)
{ 
  __asm(
"PUSH A          \n"    //1T,ѹջ
"DELAY_XUS:      \n"
"LD A,fac_us     \n"    //1T,fac_us���ص��ۼ���A
"DELAY_US_1:     \n"
"NOP             \n"    //1T,nop��ʱ
"DEC A           \n"    //1T,A--
"JRNE DELAY_US_1 \n"    //������0,����ת(2T)��DELAY_US_1����ִ��,������0,����ת(1T).
"NOP             \n"    //1T,nop��ʱ
"DECW X          \n"    //1T,x--
"JRNE DELAY_XUS  \n"    //������0,����ת(2T)��DELAY_XUS����ִ��,������0,����ת(1T).
"POP A           \n"    //1T,��ջ
); 
}

void delay_ms(unsigned int nms)
{
  unsigned int t;
  if(nms>65)
  {
    t=nms/65;
    while(t--)delay_us(65000);
    nms=nms%65;
  }
  delay_us(nms*1000);
}
*/

//����ʱ���� 
//---  ΢�뼶��ʱ--------
void delay_us(unsigned int nus) 
{ 
  for(;nus>0;nus--)
   {
   asm("nop"); //��STM8���棬16M����_nop_() ��ʱ�� 333ns
   asm("nop");
   asm("nop");
   asm("nop");
    }
} 

//---- ���뼶��ʱ����---------
void delay_ms(unsigned int nms) 
{ 
    unsigned int i; 
    while(nms--)   
    for(i=900;i>0;i--) 
    delay_us(1);  
}