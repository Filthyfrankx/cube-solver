/*****UART.h*****/
#ifndef __UART_H
#define __UART_H

void STM8_repeat(void);
#endif
/*****UART.h***/

/*****UART.c***/
#include "Arduino.h"
#include "MOTOR.c"
//#include "UART.h"
#include <SoftwareSerial.h>
SoftwareSerial mySerial(62, 63); // RX, TX定义一个新的软串口，62号引脚为RX，63号引脚为TX
char receive_data_buff[62];   //接收上位机的解魔方步骤，目前最多支持31步,由于Arduino库定义的RX缓冲区只有64字节
char send_data_buff[58];   //发送STM8的颜色识别结果到上位机
int run_mode = 0, last_step = 0, now_step = 0, run_flag = 0;
void deal_key(int ch)//发送命令给STM8
{
  switch (ch)
  {
    case 1:
      {
        mySerial.write("RST");
        digitalWrite(LED3, 1);
        return ;
      }
    case 2:
      {
        mySerial.write("RUN");
        return ;
      }
    case 3:
      {
        mySerial.write("SEN");
        digitalWrite(LED3, 0); 
        return ;
      }
  }
}

void STM8_repeat()//Arduino接收STM8颜色识别结果并写入串口
{
  static int i = 0;
  int count = 0;
  count = mySerial.available();
  delay(30);
  if (count == 0)
  {
    return ;
  }
  if (i == 0)
  {
    while (count)
    {
      count--;
      if ('b' == mySerial.read())
      {
        i++;
        break;
      }
    }
  }
  if (i == 1 && count > 54)
  {
    mySerial.readBytes(&send_data_buff[2], 54);
    if ('c' == mySerial.read())
    {
      send_data_buff[0] = 'f';
      send_data_buff[1] = 'l';
      send_data_buff[56] = 'f';
      send_data_buff[57] = 'f';
      i++;
    }
    else
      i = 0;
  }
  if (i == 2 && Serial.availableForWrite() > 58)
  {
    i = 0;
    Serial.write(send_data_buff, 58);
  }
}

void UART_Receive()//Arduino接收来自上位机的命令
{
  static int i = 0;
  int count = 0, len = 0;
  count = Serial.available();
  if (count == 0)
  {
    return ;
  }
  switch (i)
  {
    case 0:
      {
        while (count)
        {
          count--;
          if ('f' == Serial.read()) //head
          {
            i++;
            break;
          }
        }
      }
    case 1:
      {
        if (count > 0)
        {
          run_mode = Serial.read();
          count--;
          i++;
        }
      }
    case 2:
      {
        if (count > 0)
        {
          len = Serial.read();
          count--;
          i++;
        }
      }
    case 3:
      {
        if (count > len && len != 0)
        {
          Serial.readBytes(receive_data_buff, len);
          if ('c' == Serial.read()) //tail
          {
            last_step = len;
          }
          i = 0;
        }
      }
  }
}
/*****UART.c***/

/*****MAIN.c***/
#include "Arduino.h"
//#include "MOTOR.c"
//#include "UART.c"

void setup() {
  motor_init();
  GPIO_init();
  Serial.begin(115200);
  mySerial.begin(57600);
}

void loop() {
  STM8_repeat();//Arduino接收STM8颜色识别结果并写入串口
  Get_key();
  if (last_step != 0)
  {
    if (run_mode == 0x31) //连续
    {
      for (int i = 0; i < last_step; i += 2)
      {
        motor_run(receive_data_buff[i], receive_data_buff[i + 1]);
        delay(500);
      }
      //finish
      last_step = 0;
      Serial.flush();
    }
    else if (run_mode == 0x32 && run_flag == 1) //单步
    {
      motor_run(receive_data_buff[now_step], receive_data_buff[now_step + 1]);
      now_step += 2;
      if (now_step >= last_step) //finish
      {
        now_step = 0;
        last_step = 0;
        Serial.flush();
      }
      run_flag = 0;
    }
  }
  else
    UART_Receive();//Arduino接收来自上位机的命令
  delay(100);
}
