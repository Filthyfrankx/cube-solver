#include"process.h"
#include"iostm8s903k3.h"
#include"tcs34725.h"
#include"delay.h"

#define YELLOW  'D'        
#define BLUE    'B'        
#define WHITE   'U'       
#define GREEN   'F'        
#define RED     'R'        
#define ORANGE  'L'        

u8 Send_Buff[6][9];               //识别的魔方六个面数组集
u8 Colour_data[10];               //十个颜色传感器的数组
u8 start_flag=0;                
u8 send_flag=0;

void Reset_Send_Buff()
{
  u8 i,j;
  for(i=0;i<6;i++)
    for(j=0;j<9;j++)
      Send_Buff[i][j]=0;
}

void uart1_init()   //pd5  tx  pd6  rx  UART1
{
    CLK_PCKENR1 |= 0X04;  //使能USART1时钟
    PD_DDR &= ~(1<<6); // PD6 设置为RX 
    PD_CR1 |= (1<<6); 
    PD_CR2 &= ~(1<<6); //上拉输入
    PD_DDR |= (1<<5); // PD5 设置为TX 
    PD_CR1 |= (1<<5); // 推挽输出 
    PD_CR2 |= (1<<5); // 在输出模式 DDR=1 时，速度 10MHz //输出模式
    PD_ODR |= (1<<5);//发送角默认高电平
    UART1_CR2 = 0; // 禁止 UART 发送和接收
    UART1_CR1=0x00;    //8位数据，不使用奇偶效验，并且使能UART1 
    UART1_CR3=0x00;       //1位停止位，不开启同步模式
 // 必须先写BRR2
  // 例如对于波特率位115200时，分频系数=16000000/115200=139
  // 对应的十六进制数为008B，BBR1=08,BBR2=0B
 
 // 例如对于波特率位57600时，分频系数=16000000/57600=278
  // 对应的十六进制数为0x0116，BBR1=0x11,BBR2=0x06
    UART1_BRR2=0x06;
    UART1_BRR1=0x11; 
    UART1_CR2=0x2c;    //使能发送和接收功能，并开启接收中断
}

void UART1_SendChar(unsigned char ch) 
{ 
  while((UART1_SR & 0x80) == 0x00); //查询发送缓冲区的字节是否已经发送出去 
  UART1_DR = ch; // 将要发送的字符送到数据寄存器 
}

#pragma vector = UART1_RXNE_vector
__interrupt void UART1_RXNE_IRQHandler(void)
{
  unsigned char res;
  static unsigned char i=0;
  res = UART1_DR;
  UART1_SendChar(res);
  switch(i)
  {
  	case 0:
	if((res=='R')||(res=='S'))
          i++;
	break;
	case 1:
	if(res=='S')
	  i++;
	else if(res=='U')
	  i=3;
        else if(res=='E')
          i=4;
	else 
	  i=0;
	break;
	case 2:
	if(res=='T')
	{
          Reset_Send_Buff();
	  LED_R_ON;
	  LED_G_ON;
	  LED_B_ON;
	  IIC_SDA1_ON;
	  IIC_SDA2_ON;
	  IIC_SDA3_ON;
	}
	i=0;
	break;
	case 3:
	if(res=='N')
	  start_flag=1;
	i=0;
	break;
        case 4:
	if(res=='N')
	  send_flag=1;
	i=0;
	break;
  }
  return;
}

void Send_Data()
{
  u8 i,j;
  for(i=0;i<6;i++)
    for(j=0;j<9;j++)
    {
      if(Send_Buff[i][j]==0)
       return ;
    } 
  //uart_send
  UART1_SendChar('a');//f
  delay_ms(1);
  UART1_SendChar('b');//l
  delay_ms(1);
  for(i=0;i<6;i++)
    for(j=0;j<9;j++)
    {
      UART1_SendChar(Send_Buff[i][j]);
    }
  UART1_SendChar('c');//f
  delay_ms(1);
  UART1_SendChar('d');//f
  delay_ms(1);
}

void Location_Change()          //仅适用于TCS34725
{  //To Uart Send Data
  switch(Colour_data[5])
  {
    case YELLOW:
    {
      switch(Colour_data[0])
      {
        case BLUE:
        {
          Send_Buff[0][0]=Colour_data[1];
          Send_Buff[0][3]=Colour_data[2];
          Send_Buff[0][6]=Colour_data[3];
          Send_Buff[0][1]=Colour_data[4];
          Send_Buff[0][4]=Colour_data[5];
          Send_Buff[0][7]=Colour_data[6];
          Send_Buff[0][2]=Colour_data[7];
          Send_Buff[0][5]=Colour_data[8];
          Send_Buff[0][8]=Colour_data[9];
          break;
        }
        case GREEN:
        {
          Send_Buff[0][8]=Colour_data[1];
          Send_Buff[0][5]=Colour_data[2];
          Send_Buff[0][2]=Colour_data[3];
          Send_Buff[0][7]=Colour_data[4];
          Send_Buff[0][4]=Colour_data[5];
          Send_Buff[0][1]=Colour_data[6];
          Send_Buff[0][6]=Colour_data[7];
          Send_Buff[0][3]=Colour_data[8];
          Send_Buff[0][0]=Colour_data[9];
          break;
        }
        case RED:
        {
          Send_Buff[0][6]=Colour_data[1];
          Send_Buff[0][7]=Colour_data[2];
          Send_Buff[0][8]=Colour_data[3];
          Send_Buff[0][3]=Colour_data[4];
          Send_Buff[0][4]=Colour_data[5];
          Send_Buff[0][5]=Colour_data[6];
          Send_Buff[0][0]=Colour_data[7];
          Send_Buff[0][1]=Colour_data[8];
          Send_Buff[0][2]=Colour_data[9];
          break;
        }
        case ORANGE:
        {
          Send_Buff[0][2]=Colour_data[1];
          Send_Buff[0][1]=Colour_data[2];
          Send_Buff[0][0]=Colour_data[3];
          Send_Buff[0][5]=Colour_data[4];
          Send_Buff[0][4]=Colour_data[5];
          Send_Buff[0][3]=Colour_data[6];
          Send_Buff[0][8]=Colour_data[7];
          Send_Buff[0][7]=Colour_data[8];
          Send_Buff[0][6]=Colour_data[9];
          break;
        }
        default:
          return ;
      }
	  IIC_SDA1_OFF;
	  return ;
    }
    case BLUE:
    {
      switch(Colour_data[0])
      {
        case YELLOW:
        {
          Send_Buff[1][0]=Colour_data[1];
          Send_Buff[1][3]=Colour_data[2];
          Send_Buff[1][6]=Colour_data[3];
          Send_Buff[1][1]=Colour_data[4];
          Send_Buff[1][4]=Colour_data[5];
          Send_Buff[1][7]=Colour_data[6];
          Send_Buff[1][2]=Colour_data[7];
          Send_Buff[1][5]=Colour_data[8];
          Send_Buff[1][8]=Colour_data[9];
          break;
        }
        case WHITE:
        {
          Send_Buff[1][8]=Colour_data[1];
          Send_Buff[1][5]=Colour_data[2];
          Send_Buff[1][2]=Colour_data[3];
          Send_Buff[1][7]=Colour_data[4];
          Send_Buff[1][4]=Colour_data[5];
          Send_Buff[1][1]=Colour_data[6];
          Send_Buff[1][6]=Colour_data[7];
          Send_Buff[1][3]=Colour_data[8];
          Send_Buff[1][0]=Colour_data[9];
          break;
        }
        case RED:
        {
          Send_Buff[1][2]=Colour_data[1];
          Send_Buff[1][1]=Colour_data[2];
          Send_Buff[1][0]=Colour_data[3];
          Send_Buff[1][5]=Colour_data[4];
          Send_Buff[1][4]=Colour_data[5];
          Send_Buff[1][3]=Colour_data[6];
          Send_Buff[1][8]=Colour_data[7];
          Send_Buff[1][7]=Colour_data[8];
          Send_Buff[1][6]=Colour_data[9];
          break;
        }
        case ORANGE:
        {
          Send_Buff[1][6]=Colour_data[1];
          Send_Buff[1][7]=Colour_data[2];
          Send_Buff[1][8]=Colour_data[3];
          Send_Buff[1][3]=Colour_data[4];
          Send_Buff[1][4]=Colour_data[5];
          Send_Buff[1][5]=Colour_data[6];
          Send_Buff[1][0]=Colour_data[7];
          Send_Buff[1][1]=Colour_data[8];
          Send_Buff[1][2]=Colour_data[9];
          break;
        }
        default:
          return ;
      }
	  LED_B_OFF;
	  return ;
    }
    case WHITE:
    {
      switch(Colour_data[0])
      {
        case BLUE:
        {
          Send_Buff[2][8]=Colour_data[1];
          Send_Buff[2][5]=Colour_data[2];
          Send_Buff[2][2]=Colour_data[3];
          Send_Buff[2][7]=Colour_data[4];
          Send_Buff[2][4]=Colour_data[5];
          Send_Buff[2][1]=Colour_data[6];
          Send_Buff[2][6]=Colour_data[7];
          Send_Buff[2][3]=Colour_data[8];
          Send_Buff[2][0]=Colour_data[9];
          break;
        }
        case GREEN:
        {
          Send_Buff[2][0]=Colour_data[1];
          Send_Buff[2][3]=Colour_data[2];
          Send_Buff[2][6]=Colour_data[3];
          Send_Buff[2][1]=Colour_data[4];
          Send_Buff[2][4]=Colour_data[5];
          Send_Buff[2][7]=Colour_data[6];
          Send_Buff[2][2]=Colour_data[7];
          Send_Buff[2][5]=Colour_data[8];
          Send_Buff[2][8]=Colour_data[9];
          break;
        }
        case RED:
        {
          Send_Buff[2][2]=Colour_data[1];
          Send_Buff[2][1]=Colour_data[2];
          Send_Buff[2][0]=Colour_data[3];
          Send_Buff[2][5]=Colour_data[4];
          Send_Buff[2][4]=Colour_data[5];
          Send_Buff[2][3]=Colour_data[6];
          Send_Buff[2][8]=Colour_data[7];
          Send_Buff[2][7]=Colour_data[8];
          Send_Buff[2][6]=Colour_data[9];
          break;
        }
        case ORANGE:
        {
          Send_Buff[2][6]=Colour_data[1];
          Send_Buff[2][7]=Colour_data[2];
          Send_Buff[2][8]=Colour_data[3];
          Send_Buff[2][3]=Colour_data[4];
          Send_Buff[2][4]=Colour_data[5];
          Send_Buff[2][5]=Colour_data[6];
          Send_Buff[2][0]=Colour_data[7];
          Send_Buff[2][1]=Colour_data[8];
          Send_Buff[2][2]=Colour_data[9];
          break;
        }
        default:
          return ;
      }
	  IIC_SDA3_OFF;
	  return ;
    }
    case GREEN:
    {
      switch(Colour_data[0])
      {
        case YELLOW:
        {
          Send_Buff[3][0]=Colour_data[1];
          Send_Buff[3][3]=Colour_data[2];
          Send_Buff[3][6]=Colour_data[3];
          Send_Buff[3][1]=Colour_data[4];
          Send_Buff[3][4]=Colour_data[5];
          Send_Buff[3][7]=Colour_data[6];
          Send_Buff[3][2]=Colour_data[7];
          Send_Buff[3][5]=Colour_data[8];
          Send_Buff[3][8]=Colour_data[9];
          break;
        }
        case WHITE:
        {
          Send_Buff[3][8]=Colour_data[1];
          Send_Buff[3][5]=Colour_data[2];
          Send_Buff[3][2]=Colour_data[3];
          Send_Buff[3][7]=Colour_data[4];
          Send_Buff[3][4]=Colour_data[5];
          Send_Buff[3][1]=Colour_data[6];
          Send_Buff[3][6]=Colour_data[7];
          Send_Buff[3][3]=Colour_data[8];
          Send_Buff[3][0]=Colour_data[9];
          break;
        }
        case RED:
        {
          Send_Buff[3][6]=Colour_data[1];
          Send_Buff[3][7]=Colour_data[2];
          Send_Buff[3][8]=Colour_data[3];
          Send_Buff[3][3]=Colour_data[4];
          Send_Buff[3][4]=Colour_data[5];
          Send_Buff[3][5]=Colour_data[6];
          Send_Buff[3][0]=Colour_data[7];
          Send_Buff[3][1]=Colour_data[8];
          Send_Buff[3][2]=Colour_data[9];
          break;
        }
        case ORANGE:
        {
          Send_Buff[3][2]=Colour_data[1];
          Send_Buff[3][1]=Colour_data[2];
          Send_Buff[3][0]=Colour_data[3];
          Send_Buff[3][5]=Colour_data[4];
          Send_Buff[3][4]=Colour_data[5];
          Send_Buff[3][3]=Colour_data[6];
          Send_Buff[3][8]=Colour_data[7];
          Send_Buff[3][7]=Colour_data[8];
          Send_Buff[3][6]=Colour_data[9];
          break;
        }
        default:
          return ;
      }
	  LED_G_OFF;
	  return ;
    }
    case RED:
    {
      switch(Colour_data[0])
      {
        case YELLOW:
        {
          Send_Buff[4][0]=Colour_data[1];
          Send_Buff[4][3]=Colour_data[2];
          Send_Buff[4][6]=Colour_data[3];
          Send_Buff[4][1]=Colour_data[4];
          Send_Buff[4][4]=Colour_data[5];
          Send_Buff[4][7]=Colour_data[6];
          Send_Buff[4][2]=Colour_data[7];
          Send_Buff[4][5]=Colour_data[8];
          Send_Buff[4][8]=Colour_data[9];
          break;
        }
        case BLUE:
        {
          Send_Buff[4][6]=Colour_data[1];
          Send_Buff[4][7]=Colour_data[2];
          Send_Buff[4][8]=Colour_data[3];
          Send_Buff[4][3]=Colour_data[4];
          Send_Buff[4][4]=Colour_data[5];
          Send_Buff[4][5]=Colour_data[6];
          Send_Buff[4][0]=Colour_data[7];
          Send_Buff[4][1]=Colour_data[8];
          Send_Buff[4][2]=Colour_data[9];
          break;
        }
        case WHITE:
        {
          Send_Buff[4][2]=Colour_data[1];
          Send_Buff[4][1]=Colour_data[2];
          Send_Buff[4][0]=Colour_data[3];
          Send_Buff[4][5]=Colour_data[4];
          Send_Buff[4][4]=Colour_data[5];
          Send_Buff[4][3]=Colour_data[6];
          Send_Buff[4][8]=Colour_data[7];
          Send_Buff[4][7]=Colour_data[8];
          Send_Buff[4][6]=Colour_data[9];
          break;
        }
        case GREEN:
        {
          Send_Buff[4][8]=Colour_data[1];
          Send_Buff[4][5]=Colour_data[2];
          Send_Buff[4][2]=Colour_data[3];
          Send_Buff[4][7]=Colour_data[4];
          Send_Buff[4][4]=Colour_data[5];
          Send_Buff[4][1]=Colour_data[6];
          Send_Buff[4][6]=Colour_data[7];
          Send_Buff[4][3]=Colour_data[8];
          Send_Buff[4][0]=Colour_data[9];
          break;
        }
        default:
          return ;
      }
      	  LED_R_OFF;
	  return ;
    }
    case ORANGE:
    {
      switch(Colour_data[0])
      {
        case YELLOW:
        {
          Send_Buff[5][0]=Colour_data[1];
          Send_Buff[5][3]=Colour_data[2];
          Send_Buff[5][6]=Colour_data[3];
          Send_Buff[5][1]=Colour_data[4];
          Send_Buff[5][4]=Colour_data[5];
          Send_Buff[5][7]=Colour_data[6];
          Send_Buff[5][2]=Colour_data[7];
          Send_Buff[5][5]=Colour_data[8];
          Send_Buff[5][8]=Colour_data[9];
          break;
        }
        case BLUE:
        {
          Send_Buff[5][2]=Colour_data[1];
          Send_Buff[5][1]=Colour_data[2];
          Send_Buff[5][0]=Colour_data[3];
          Send_Buff[5][5]=Colour_data[4];
          Send_Buff[5][4]=Colour_data[5];
          Send_Buff[5][3]=Colour_data[6];
          Send_Buff[5][8]=Colour_data[7];
          Send_Buff[5][7]=Colour_data[8];
          Send_Buff[5][6]=Colour_data[9];
          break;
        }
        case WHITE:
        {
          Send_Buff[5][8]=Colour_data[1];
          Send_Buff[5][5]=Colour_data[2];
          Send_Buff[5][2]=Colour_data[3];
          Send_Buff[5][7]=Colour_data[4];
          Send_Buff[5][4]=Colour_data[5];
          Send_Buff[5][1]=Colour_data[6];
          Send_Buff[5][6]=Colour_data[7];
          Send_Buff[5][3]=Colour_data[8];
          Send_Buff[5][0]=Colour_data[9];
          break;
        }
        case GREEN:
        {
          Send_Buff[5][6]=Colour_data[1];
          Send_Buff[5][7]=Colour_data[2];
          Send_Buff[5][8]=Colour_data[3];
          Send_Buff[5][3]=Colour_data[4];
          Send_Buff[5][4]=Colour_data[5];
          Send_Buff[5][5]=Colour_data[6];
          Send_Buff[5][0]=Colour_data[7];
          Send_Buff[5][1]=Colour_data[8];
          Send_Buff[5][2]=Colour_data[9];
          break;
        }
        default:
          return ;
      }
	  IIC_SDA2_OFF;
	  return ;
      }
    }
  }

/*char Get_Colour(int H,int S,int V)//Get_colour
{
  //将HSV转换成颜色（需调试）
  if(5<H&&H<20&&50<S&&S<80&&40<V&&V<70)
    return ORANGE;        //O
  else if(200<H&&H<220&&60<S&&S<90&&40<V&&V<60)
    return BLUE;          //B
  else if(120<H&&H<150&&50<S&&S<70&&40<V&&V<60)
    return GREEN;         //G
  else if(60<H&&H<80&&40<S&&S<60&&30<V&&V<50)
    return YELLOW;        //Y
  else if(160<H&&H<200&&20<S&&S<40&&20<V&&V<50)
    return WHITE;         //W
  else if(((0<=H&&H<5)||(300<H&&H<360))&&40<S&&S<80&&40<V&&V<70)
    return RED;           //R 
  else
    return 0;
}*/

 char Get_Colour(int H,int S,int V)//Get_colour
{
  //将HSV转换成颜色（已调试）
  if(4<H&&H<20&&30<S&&S<90&&40<V&&V<70)
    return ORANGE;        //O
  else if(180<H&&H<230&&60<S&&S<90&&40<V&&V<70)
    return BLUE;          //B
  else if(120<H&&H<150&&40<S&&S<70&&40<V&&V<70)
    return GREEN;         //G
  else if(30<H&&H<100&&30<S&&S<70&&20<V&&V<50)
    return YELLOW;        //Y
  else if(140<H&&H<210&&20<S&&S<40&&20<V&&V<50)
    return WHITE;         //W
  else if(((0<=H&&H<4)||(300<H&&H<360))&&30<S&&S<80&&30<V&&V<70)
    return RED;           //R 
  else
    return 0;
}

char Min3v(int x,int y,int z)
{
    if ((x<y)&&(x<z))
        return x;
    else if((y<x)&&(y<z))
        return y;
    else if((z<x)&&(z<y))
        return z;
    else if((z==x)&&(z<y))
       return x;
    else if((y==x)&&(y<z))
       return y;
    else if((z==y)&&(z<x))
       return z;
    else 
      return 0;
} 

char Max3v(int x,int y,int z)
{
    if ((x>y)&&(x>z))
        return x;
    else if((y>x)&&(y>z))
        return y;
    else if((z>x)&&(z>y))
        return z;
    else if((z==x)&&(z>y))
       return x;
    else if((y==x)&&(y>z))
       return y;
    else if((z==y)&&(z>x))
       return z;
    else
       return 0;
}

void Get_Data_I2C()
{
  u8 ch;
  u16 R_data,G_data,B_data,C_data;
  u16 R, G, B, min, max, delta;
  double H, S, V;
  for(ch=0;ch<10;ch++)
  {
    //USE_TCS34725
    C_data = TCS34725_GetChannelData(ch,TCS34725_CDATAL);
    R_data = TCS34725_GetChannelData(ch,TCS34725_RDATAL);    
    G_data = TCS34725_GetChannelData(ch,TCS34725_GDATAL);    
    B_data = TCS34725_GetChannelData(ch,TCS34725_BDATAL);
      // r,g,b =[0,100]
     // h = [0,360], s = [0,100], v = [0,100]
     // 如果 s == 0, h = -1 
     R = R_data*100/C_data;
     G = G_data*100/C_data;
     B = B_data*100/C_data;
     min = Min3v(R,G,B);
     max = Max3v(R,G,B);
    //*****************以下为RGB转HSV部分*********************//
     V = max;           // v
     delta = max - min;
     if(max!=0)
       S=(delta/V)*100; // s
     else
       // r = g = b = 0 // s = 0
       S=0;
     if( R == max )
     {
       H=60*((float)G - B ) / delta;;
     } // 在黄色和品红之间（红色、橙色、黄色、白色）
     else if( G == max )
     { 
       H=60*(2 + ((float)B - R ) / delta);
     } // 在黄色和青色之间（绿色、黄色、白色）
     else if (B == max )
     { 
       H=60*(4 + ((float)R - G ) / delta);
     } // 在青色和品红之间（蓝色、绿色、白色）
     else 
     {   
       H=0;
     }
      if(H<0)
        H += 360;
      Colour_data[ch]=Get_Colour(H,S,V);  
}
Location_Change();
}