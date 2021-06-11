#ifndef __MOTOR_H
#define __MOTOR_H

#define MOTOR1_A 7
#define MOTOR1_B 6
#define MOTOR1_C 5
#define MOTOR1_D 4

#define MOTOR2_A 11
#define MOTOR2_B 10
#define MOTOR2_C 9
#define MOTOR2_D 8

#define MOTOR3_A 15
#define MOTOR3_B 14
#define MOTOR3_C 13
#define MOTOR3_D 12

#define MOTOR4_A 23
#define MOTOR4_B 22
#define MOTOR4_C 21
#define MOTOR4_D 20

#define MOTOR5_A 27
#define MOTOR5_B 26
#define MOTOR5_C 25
#define MOTOR5_D 24

#define Light1 32
#define Light2 33
#define Light3 34
#define Light4 35
#define Light5 36

void motor_init(void);
void motor_run(int ch,int mode);

#endif

/***GPIO.h***/
#ifndef __GPIO_H
#define __GPIO_H

#define KEY1  40
#define KEY2  41
#define KEY3  42
#define LED1  37
#define LED2  38
#define LED3  39

void GPIO_init(void);
void deal_key(int ch);

#endif
