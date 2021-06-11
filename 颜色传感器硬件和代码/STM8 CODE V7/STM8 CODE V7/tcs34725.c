#include"tcs34725.h"
#include"delay.h"

void TCS_SDA_DIR_IN(u8 ch)
{
  //悬浮输入}//输入模式
  switch(ch)
  {
  	case 1:
  	PF_DDR &= ~(1<<TCS_SDA1_PIN);    //4 //PF4
  	PF_CR1 &= ~(1<<TCS_SDA1_PIN);
  	PF_CR2 &= ~(1<<TCS_SDA1_PIN); 
	return ;
	case 2:
  	PB_DDR &= ~(1<<TCS_SDA2_PIN);    //7  //PB7
  	PB_CR1 &= ~(1<<TCS_SDA2_PIN);
  	PB_CR2 &= ~(1<<TCS_SDA2_PIN); 
	return ;
	case 3:
  	PB_DDR &= ~(1<<TCS_SDA3_PIN);     //6  //PB6
  	PB_CR1 &= ~(1<<TCS_SDA3_PIN);
  	PB_CR2 &= ~(1<<TCS_SDA3_PIN); 
	return ;
	case 4:
  	PB_DDR &= ~(1<<TCS_SDA4_PIN);    // 5  //PB5
  	PB_CR1 &= ~(1<<TCS_SDA4_PIN);
  	PB_CR2 &= ~(1<<TCS_SDA4_PIN); 
	return ;
	case 5:
  	PB_DDR &= ~(1<<TCS_SDA5_PIN);     //4  //PB4
  	PB_CR1 &= ~(1<<TCS_SDA5_PIN);
  	PB_CR2 &= ~(1<<TCS_SDA5_PIN); 
	return ;
	case 6:
  	PB_DDR &= ~(1<<TCS_SDA6_PIN);     //3  //PB3
  	PB_CR1 &= ~(1<<TCS_SDA6_PIN);
  	PB_CR2 &= ~(1<<TCS_SDA6_PIN); 
	return ;
	case 7:
  	PB_DDR &= ~(1<<TCS_SDA7_PIN);     //2  //PB2
  	PB_CR1 &= ~(1<<TCS_SDA7_PIN);
  	PB_CR2 &= ~(1<<TCS_SDA7_PIN); 
	return ;
	case 8:
  	PB_DDR &= ~(1<<TCS_SDA8_PIN);    // 1  //PB1
  	PB_CR1 &= ~(1<<TCS_SDA8_PIN);
  	PB_CR2 &= ~(1<<TCS_SDA8_PIN); 
	return ;
	case 9:
  	PB_DDR &= ~(1<<TCS_SDA9_PIN);    // 0  //PB0
  	PB_CR1 &= ~(1<<TCS_SDA9_PIN);
  	PB_CR2 &= ~(1<<TCS_SDA9_PIN); 
	return ;
	case 0:
  	PC_DDR &= ~(1<<TCS_SDA0_PIN);    // 4  //PC4
  	PC_CR1 &= ~(1<<TCS_SDA0_PIN);
  	PC_CR2 &= ~(1<<TCS_SDA0_PIN); 
	return ;
  	default:
	  return ;
  }
}
void TCS_SDA_DIR_OUT(u8 ch)
{
  switch(ch)
  {
      case 1:
      PF_DDR |= (1<<TCS_SDA1_PIN); /* 设置为输出 */
      PF_CR1 |= (1<<TCS_SDA1_PIN); /* 推挽输出 */
      PF_CR2 &= ~(1<<TCS_SDA1_PIN); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
      return ;
      case 2:
      PB_DDR |= (1<<TCS_SDA2_PIN); /* 设置为输出 */
      PB_CR1 |= (1<<TCS_SDA2_PIN); /* 推挽输出 */
      PB_CR2 &= ~(1<<TCS_SDA2_PIN); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
      return ;
      case 3:
      PB_DDR |= (1<<TCS_SDA3_PIN); /* 设置为输出 */
      PB_CR1 |= (1<<TCS_SDA3_PIN); /* 推挽输出 */
      PB_CR2 &= ~(1<<TCS_SDA3_PIN); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
      return ;
      case 4:
      PB_DDR |= (1<<TCS_SDA4_PIN); /* 设置为输出 */
      PB_CR1 |= (1<<TCS_SDA4_PIN); /* 推挽输出 */
      PB_CR2 &= ~(1<<TCS_SDA4_PIN); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
      return ;
      case 5:
      PB_DDR |= (1<<TCS_SDA5_PIN); /* 设置为输出 */
      PB_CR1 |= (1<<TCS_SDA5_PIN); /* 推挽输出 */
      PB_CR2 &= ~(1<<TCS_SDA5_PIN); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
      return ;
      case 6:
      PB_DDR |= (1<<TCS_SDA6_PIN); /* 设置为输出 */
      PB_CR1 |= (1<<TCS_SDA6_PIN); /* 推挽输出 */
      PB_CR2 &= ~(1<<TCS_SDA6_PIN); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
      return ;
      case 7:
      PB_DDR |= (1<<TCS_SDA7_PIN); /* 设置为输出 */
      PB_CR1 |= (1<<TCS_SDA7_PIN); /* 推挽输出 */
      PB_CR2 &= ~(1<<TCS_SDA7_PIN); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
      return ;
      case 8:
      PB_DDR |= (1<<TCS_SDA8_PIN); /* 设置为输出 */
      PB_CR1 |= (1<<TCS_SDA8_PIN); /* 推挽输出 */
      PB_CR2 &= ~(1<<TCS_SDA8_PIN); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
      return ;
      case 9:
      PB_DDR |= (1<<TCS_SDA9_PIN); /* 设置为输出 */
      PB_CR1 |= (1<<TCS_SDA9_PIN); /* 推挽输出 */
      PB_CR2 &= ~(1<<TCS_SDA9_PIN); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
      return ;
      case 0:
      PC_DDR |= (1<<TCS_SDA0_PIN); /* 设置为输出 */
      PC_CR1 |= (1<<TCS_SDA0_PIN); /* 推挽输出 */
      PC_CR2 &= ~(1<<TCS_SDA0_PIN); /* 在输出模式 DDR=1 时，速度 2MHz *///输出模式
      return ;
  }
}
u8 TCS_SDA_READ(u8 ch)
{
  switch(ch)
  {
  	case 1:
  	return (PF_IDR & (1<<TCS_SDA1_PIN)) == (1<<TCS_SDA1_PIN);
	case 2:
  	return (PB_IDR & (1<<TCS_SDA2_PIN)) == (1<<TCS_SDA2_PIN);
	case 3:
  	return (PB_IDR & (1<<TCS_SDA3_PIN)) == (1<<TCS_SDA3_PIN);
	case 4:
  	return (PB_IDR & (1<<TCS_SDA4_PIN)) == (1<<TCS_SDA4_PIN);
	case 5:
  	return (PB_IDR & (1<<TCS_SDA5_PIN)) == (1<<TCS_SDA5_PIN);
	case 6:
  	return (PB_IDR & (1<<TCS_SDA6_PIN)) == (1<<TCS_SDA6_PIN);
	case 7:
  	return (PB_IDR & (1<<TCS_SDA7_PIN)) == (1<<TCS_SDA7_PIN);
	case 8:
  	return (PB_IDR & (1<<TCS_SDA8_PIN)) == (1<<TCS_SDA8_PIN);
	case 9:
  	return (PB_IDR & (1<<TCS_SDA9_PIN)) == (1<<TCS_SDA9_PIN);
	case 0:
  	return (PC_IDR & (1<<TCS_SDA0_PIN)) == (1<<TCS_SDA0_PIN);
  	default:
	return 0;
  }
}
void TCS_SDA_SET(u8 ch)
{
  switch(ch)
  {
  	case 1:
	PF_ODR |= (1<<TCS_SDA1_PIN); /* H */
	return;
	case 2:
	PB_ODR |= (1<<TCS_SDA2_PIN); /* H */
	return;
	case 3:
	PB_ODR |= (1<<TCS_SDA3_PIN); /* H */
	return;
	case 4:
	PB_ODR |= (1<<TCS_SDA4_PIN); /* H */
	return;
	case 5:
	PB_ODR |= (1<<TCS_SDA5_PIN); /* H */
	return;
	case 6:
	PB_ODR |= (1<<TCS_SDA6_PIN); /* H */
	return;
	case 7:
	PB_ODR |= (1<<TCS_SDA7_PIN); /* H */
	return;
	case 8:
	PB_ODR |= (1<<TCS_SDA8_PIN); /* H */
	return;
	case 9:
	PB_ODR |= (1<<TCS_SDA9_PIN); /* H */
	return;
	case 0:
	PC_ODR |= (1<<TCS_SDA0_PIN); /* H */
	return;
  }
}
void TCS_SDA_RESET(u8 ch)
{
  switch(ch)
  {
  	case 1:
	PF_ODR &= ~(1<<TCS_SDA1_PIN);
	return ;
	case 2:
	PB_ODR &= ~(1<<TCS_SDA2_PIN);
	return ;
	case 3:
	PB_ODR &= ~(1<<TCS_SDA3_PIN);
	return ;
	case 4:
	PB_ODR &= ~(1<<TCS_SDA4_PIN);
	return ;
	case 5:
	PB_ODR &= ~(1<<TCS_SDA5_PIN);
	return ;
	case 6:
	PB_ODR &= ~(1<<TCS_SDA6_PIN);
	return ;
	case 7:
	PB_ODR &= ~(1<<TCS_SDA7_PIN);
	return ;
	case 8:
	PB_ODR &= ~(1<<TCS_SDA8_PIN);
	return ;
	case 9:
	PB_ODR &= ~(1<<TCS_SDA9_PIN);
	return ;
	case 0:
	PC_ODR &= ~(1<<TCS_SDA0_PIN);
	return ;
  }
}

void TCS34725_I2C_Start(u8 ch)
{
    TCS_SDA_DIR_OUT(ch);//sda线输出
    TCS_SDA_SET(ch);
    TCS_SCL_SET;
    delay_us(4);
    TCS_SDA_RESET(ch);//START:when CLK is high,DATA change form high to low 
    delay_us(4);
    TCS_SCL_RESET;//钳住I2C总线，准备发送或接收数据 
}
//产生I2C停止信号
void TCS34725_I2C_Stop(u8 ch)
{
    TCS_SDA_DIR_OUT(ch);//sda线输出
    TCS_SCL_RESET;
    TCS_SDA_RESET(ch);//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    TCS_SCL_SET;
    TCS_SDA_SET(ch);//发送I2C总线结束信号
    delay_us(4);                                   
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
unsigned int TCS34725_I2C_Wait_ACK(u8 ch)
{
    unsigned int timeOut = 0;
    
    TCS_SDA_DIR_IN(ch);//SDA设置为输入  
    TCS_SDA_SET(ch);//?????
    delay_us(1);
    TCS_SCL_SET; 
    delay_us(1);
    while(TCS_SDA_READ(ch))
    {
        timeOut++;
        if(timeOut > 250)
        {
            TCS34725_I2C_Stop(ch);
            return 1;
        }
    }
    TCS_SCL_RESET;//时钟输出0
            
    return 0;    
} 

//产生ACK应答
void TCS34725_I2C_ACK(u8 ch)
{
    TCS_SCL_RESET;
    TCS_SDA_DIR_OUT(ch);//sda线输出
    TCS_SDA_RESET(ch);
    delay_us(2);
    TCS_SCL_SET;
    delay_us(2);
    TCS_SCL_RESET;
}

//不产生ACK应答 
void TCS34725_I2C_NACK(u8 ch)
{
    TCS_SCL_RESET;
    TCS_SDA_DIR_OUT(ch);//sda线输出
    TCS_SDA_SET(ch);
    delay_us(2);
    TCS_SCL_SET;
    delay_us(2);
    TCS_SCL_RESET;
}    

//I2C发送一个字节
void TCS34725_I2C_Send_Byte(u8 ch,unsigned int byte)
{
    unsigned int i = 0;
    
    TCS_SDA_DIR_OUT(ch);//sda线输出
    TCS_SCL_RESET;//拉低时钟开始数据传输
    for(i = 0; i < 8; i++)
    {
        if(((byte & 0x80) >> 7) == 0x01 )      //
           TCS_SDA_SET(ch); 
          else TCS_SDA_RESET(ch);
        byte <<= 1;
        delay_us(2);
        TCS_SCL_SET;
        delay_us(2);
        TCS_SCL_RESET;
        delay_us(2);
    } 
} 

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
unsigned int TCS34725_I2C_Read_Byte(u8 ch,unsigned int ack)
{
    unsigned int i,receive = 0;
    
    TCS_SDA_DIR_IN(ch);
    for(i = 0; i < 8; i++)
    {
        TCS_SCL_RESET;
        delay_us(2);
        TCS_SCL_SET;
        receive <<= 1;
        if(TCS_SDA_READ(ch)) receive++;
        delay_us(1);
    }
    if (!ack) TCS34725_I2C_NACK(ch);//发送nACK
    else TCS34725_I2C_ACK(ch); //发送ACK 
    
    return receive;
}

void TCS34725_I2C_Write(u8 ch,unsigned int slaveAddress, unsigned char* dataBuffer,unsigned int bytesNumber, unsigned int stopBit)
{
    unsigned char i = 0;
    
    TCS34725_I2C_Start(ch);
    TCS34725_I2C_Send_Byte(ch,(slaveAddress << 1) | 0x00);       //发送从机地址写命令
    TCS34725_I2C_Wait_ACK(ch);
    for(i = 0; i < bytesNumber; i++)
    {
        TCS34725_I2C_Send_Byte(ch,*(dataBuffer + i));
        TCS34725_I2C_Wait_ACK(ch);
    }
    if(stopBit == 1) TCS34725_I2C_Stop(ch);
}

void TCS34725_I2C_Read(u8 ch,unsigned int slaveAddress, unsigned char* dataBuffer, unsigned int bytesNumber, unsigned int stopBit)
{
    unsigned char i = 0;
    
    TCS34725_I2C_Start(ch);
    TCS34725_I2C_Send_Byte(ch,(slaveAddress << 1) | 0x01);       //发送从机地址读命令
    TCS34725_I2C_Wait_ACK(ch);
    for(i = 0; i < bytesNumber; i++)
    {
        if(i == bytesNumber - 1)
        {
            *(dataBuffer + i) = TCS34725_I2C_Read_Byte(ch,0);//读取的最后一个字节发送NACK
        }
        else
        {
            *(dataBuffer + i) = TCS34725_I2C_Read_Byte(ch,1);
        }
    }
    if(stopBit == 1) TCS34725_I2C_Stop(ch);
}



void TCS34725_Write(u8 ch,unsigned char subAddr, unsigned char* dataBuffer, unsigned char bytesNumber)
{
    unsigned char sendBuffer[10] = {0, };
    unsigned char byte = 0;
    
    sendBuffer[0] = subAddr | TCS34725_COMMAND_BIT;
    for(byte = 1; byte <= bytesNumber; byte++)
    {
        sendBuffer[byte] = dataBuffer[byte - 1];
    }
    TCS34725_I2C_Write(ch,TCS34725_ADDRESS, sendBuffer, bytesNumber + 1, 1);
}

void TCS34725_Read(u8 ch,unsigned char subAddr, unsigned char* dataBuffer, unsigned char bytesNumber)
{
    subAddr |= TCS34725_COMMAND_BIT;
    
    TCS34725_I2C_Write(ch,TCS34725_ADDRESS, (unsigned char*)&subAddr, 1, 0);
    TCS34725_I2C_Read(ch,TCS34725_ADDRESS, dataBuffer, bytesNumber, 1);
}

unsigned char TCS34725_Init(u8 ch)
{
    unsigned char status[1] = {0};
    
    TCS_SDA_DIR_OUT(ch);
    TCS_SCL_DIR_OUT();
    TCS_SDA_SET(ch);
    TCS_SCL_SET;
    TCS34725_Read(ch,TCS34725_ID, status, 1);
    return status[0];
}

void TCS34725_Enable(u8 ch)
{
    unsigned char cd = TCS34725_INTEGRATIONTIME_240MS;
    TCS34725_Write(ch,TCS34725_ATIME, &cd, 1);
    cd = TCS34725_GAIN_4X;
   TCS34725_Write(ch,TCS34725_CONTROL, &cd, 1);
    
    
    unsigned char cmd = TCS34725_ENABLE_PON;
    
    TCS34725_Write(ch,TCS34725_ENABLE, &cmd, 1);
    //delay_ms(3);//延时应该放在设置AEN之后
    cmd = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
    TCS34725_Write(ch,TCS34725_ENABLE, &cmd, 1);
    delay_ms(3);
}
unsigned int TCS34725_GetChannelData(u8 ch,unsigned char reg)
{
    unsigned char tmp[2] = {0,0};
    unsigned int data = 0;
    
    TCS34725_Read(ch,reg, tmp, 2);
    data = ((unsigned int)tmp[1] << 8) | tmp[0];
    
    return data;
}
void TCS34725_Init_All()
{
  u8 ch=1;
  for(ch=0;ch<10;ch++)
  {
    TCS_SDA_DIR_OUT(ch);
    TCS_SCL_DIR_OUT();
    TCS_SDA_SET(ch);
    TCS_SCL_SET;
    TCS34725_Enable(ch);
    delay_ms(10);
  }
}