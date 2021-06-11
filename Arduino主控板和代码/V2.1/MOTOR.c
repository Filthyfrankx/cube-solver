#include "MOTOR.h"
#include "Arduino.h"
#define DELAY_TIME 1.5             //延时时间x ms（可改动）

const int eighttb[8][4]={        
{1,0,0,0},
{1,0,1,0},
{0,0,1,0},
{0,1,1,0},
{0,1,0,0},
{0,1,0,1},
{0,0,0,1},
{1,0,0,1}};
const int eighttb_back[8][4]={    
{0,1,0,0},
{0,1,0,1},
{0,0,0,1},
{1,0,0,1},
{1,0,0,0},
{1,0,1,0},
{0,0,1,0},
{0,1,1,0}};
void motor_init()
{
  pinMode(MOTOR1_A, OUTPUT);  //7
  pinMode(MOTOR1_B, OUTPUT);  //6
  pinMode(MOTOR1_C, OUTPUT);  //5
  pinMode(MOTOR1_D, OUTPUT);  //4
  
  pinMode(MOTOR2_A, OUTPUT);  //11
  pinMode(MOTOR2_B, OUTPUT);  //10
  pinMode(MOTOR2_C, OUTPUT);  //9
  pinMode(MOTOR2_D, OUTPUT);  //8
  
  pinMode(MOTOR3_A, OUTPUT);  //15
  pinMode(MOTOR3_B, OUTPUT);  //14
  pinMode(MOTOR3_C, OUTPUT);  //13
  pinMode(MOTOR3_D, OUTPUT);  //12
  
  pinMode(MOTOR4_A, OUTPUT);  //23
  pinMode(MOTOR4_B, OUTPUT);  //22
  pinMode(MOTOR4_C, OUTPUT);  //21
  pinMode(MOTOR4_D, OUTPUT);  //20
  
  pinMode(MOTOR5_A, OUTPUT);  //27
  pinMode(MOTOR5_B, OUTPUT);  //26
  pinMode(MOTOR5_C, OUTPUT);  //25
  pinMode(MOTOR5_D, OUTPUT);  //24

  pinMode(Light1, INPUT);  //32
  pinMode(Light2, INPUT);  //33
  pinMode(Light3, INPUT);  //34
  pinMode(Light4, INPUT);  //35
  pinMode(Light5, INPUT);  //36
}
void motor_run(int ch,int mode)
{
  int A,B,C,D,i,j,Flag,count;
  switch(ch)
  {
    case 'D':
    {
      A=MOTOR1_A;
      B=MOTOR1_B;
      C=MOTOR1_C;
      D=MOTOR1_D;
      Flag=Light1;
      break;
    }
    case 'L':
    {
      A=MOTOR2_A;
      B=MOTOR2_B;
      C=MOTOR2_C;
      D=MOTOR2_D;
      Flag=Light2;
      break;
    }
    case 'R':
    {
      A=MOTOR3_A;
      B=MOTOR3_B;
      C=MOTOR3_C;
      D=MOTOR3_D;
      Flag=Light3;
      break;
    }
    case 'F':
    {
      A=MOTOR4_A;
      B=MOTOR4_B;
      C=MOTOR4_C;
      D=MOTOR4_D;
      Flag=Light4;
      break;
    }
    case 'B':
    {
      A=MOTOR5_A;
      B=MOTOR5_B;
      C=MOTOR5_C;
      D=MOTOR5_D;
      Flag=Light5;
      break;
    }
    default:
      return ;
  }
  if(mode==1||mode==3)   //90（可改动）
    i=90;  //给低一点避免过节 100  130  120 110
  else if(mode==2)        //180（可改动）
    i=180;
  else
    return ;
  for(j=0;j<i;j++)
  {
    if(mode == 3)     //逆时针
    {
      digitalWrite(A, eighttb[7-(j%8)][0]);
      digitalWrite(B, eighttb[7-(j%8)][1]);
      digitalWrite(C, eighttb[7-(j%8)][2]);
      digitalWrite(D, eighttb[7-(j%8)][3]);
    }
    else              //顺时针
    {
      digitalWrite(A, eighttb[(j%8)][0]);
      digitalWrite(B, eighttb[(j%8)][1]);
      digitalWrite(C, eighttb[(j%8)][2]);
      digitalWrite(D, eighttb[(j%8)][3]);
    }
    //delayMicroseconds(800);
   delay(DELAY_TIME);
  }
  while(1)
  {
    if(j==8)
      j=0;
    else
      j++;
    if(digitalRead(Flag)==0)
    {
      i = j +10;
      for(;j<i;j++)
      {
        if(mode == 3)     //逆时针
        {
          digitalWrite(A, eighttb[7-(j%8)][0]);
          digitalWrite(B, eighttb[7-(j%8)][1]);
          digitalWrite(C, eighttb[7-(j%8)][2]);
          digitalWrite(D, eighttb[7-(j%8)][3]);
        }
        else              //顺时针
        {
          digitalWrite(A, eighttb[(j%8)][0]);
          digitalWrite(B, eighttb[(j%8)][1]);
          digitalWrite(C, eighttb[(j%8)][2]);
          digitalWrite(D, eighttb[(j%8)][3]);
        }
        //delayMicroseconds(800);
       delay(DELAY_TIME);
      }
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW); 
      return ;
    }
    if(mode == 3)     //逆时针
    {
      digitalWrite(A, eighttb[7-(j%8)][0]);
      digitalWrite(B, eighttb[7-(j%8)][1]);
      digitalWrite(C, eighttb[7-(j%8)][2]);
      digitalWrite(D, eighttb[7-(j%8)][3]);
    }
    else              //顺时针
    {
      digitalWrite(A, eighttb[(j%8)][0]);
      digitalWrite(B, eighttb[(j%8)][1]);
      digitalWrite(C, eighttb[(j%8)][2]);
      digitalWrite(D, eighttb[(j%8)][3]);
    }
    //delayMicroseconds(800);
   delay(DELAY_TIME);
  }
}

/***GPIO.c***/
void GPIO_init()
{
  pinMode(KEY1, INPUT);
  pinMode(KEY2, INPUT);
  pinMode(KEY3, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED1, 1);//点亮LED灯
  digitalWrite(LED2, 1);
  digitalWrite(LED3, 1);
}

void Get_key()
{
  int key1_valve=HIGH,key2_valve=HIGH,key3_valve=HIGH;
  int key_valve=HIGH;
  key_valve=digitalRead(KEY1);
  if(key_valve==LOW&&key1_valve==HIGH)
  {
      delay(40);
      key1_valve=LOW;
       if(key_valve==LOW)
       {
        deal_key(1);
        key_valve=HIGH;
        }
  }
  else
  key1_valve==HIGH;
  
  key_valve=digitalRead(KEY2);
  if(key_valve==LOW&&key2_valve==HIGH)
  {
      delay(40);
      key2_valve=LOW;
       if(key_valve==LOW)
       {
        deal_key(2);
        key_valve=HIGH;
        }
  }
  else
  key2_valve==HIGH;
  
  key_valve=digitalRead(KEY3);
  if(key_valve==LOW&&key3_valve==HIGH)
  {
      delay(40);
      key3_valve=LOW;
       if(key_valve==LOW)
       {
        deal_key(3);
        key_valve=HIGH;
        }
  }
  else
  key3_valve==HIGH;
}
