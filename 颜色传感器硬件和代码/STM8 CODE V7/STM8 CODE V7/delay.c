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
//该延时函数不方便debug
void delay_us(unsigned int nus)
{ 
  __asm(
"PUSH A          \n"    //1T,压栈
"DELAY_XUS:      \n"
"LD A,fac_us     \n"    //1T,fac_us加载到累加器A
"DELAY_US_1:     \n"
"NOP             \n"    //1T,nop延时
"DEC A           \n"    //1T,A--
"JRNE DELAY_US_1 \n"    //不等于0,则跳转(2T)到DELAY_US_1继续执行,若等于0,则不跳转(1T).
"NOP             \n"    //1T,nop延时
"DECW X          \n"    //1T,x--
"JRNE DELAY_XUS  \n"    //不等于0,则跳转(2T)到DELAY_XUS继续执行,若等于0,则不跳转(1T).
"POP A           \n"    //1T,出栈
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

//新延时函数 
//---  微秒级延时--------
void delay_us(unsigned int nus) 
{ 
  for(;nus>0;nus--)
   {
   asm("nop"); //在STM8里面，16M晶振，_nop_() 延时了 333ns
   asm("nop");
   asm("nop");
   asm("nop");
    }
} 

//---- 毫秒级延时程序---------
void delay_ms(unsigned int nms) 
{ 
    unsigned int i; 
    while(nms--)   
    for(i=900;i>0;i--) 
    delay_us(1);  
}