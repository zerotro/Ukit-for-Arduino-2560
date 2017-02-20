#include"UkitMotor.h"
#include "SR04.h"
#define TRIG_PIN 30
#define ECHO_PIN 32
UkitMotor motor;
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);

bool Rdec;
bool Ldec;
int Find;
int num1,num2,num3,num4,num5;
unsigned char tData[32] ;
long a;
long b;
long c;
void setup() {
    // put your setup code here, to run once:
  Serial.setTimeout(2);  //设置超时ms
  Serial.begin(115200);
  delay(1000);  //开机延时
  motor.check_servo();  //获取舵机个数,列表
  //motor.Servo_Tx(0xFA,12,4,0x01,120<<0 | 200<<8 );

  delay(2);
  Serial.print(Ldec,Rdec);
  delay(100);
        tData[0] = 120;
        tData[1] = (100/20);
  motor.Servo_Tx(0xFA,12,4,0x01,tData);

}

void loop() {
  //turnL(765);
  Tandtenfindline(765,0);
// findline(765);

}
void Tandtenfindline(int speed,int dir)
{
  if(dir==0)
  {
  findline(speed);
  if((num1==0)&&(num5==0))
  { 
    forward(speed);
    delay(200);
    turnL(speed);
    delay(1000);
   // stop();
    while(1){
        num3=digitalRead(11);  //循环判断中间传感器度数，
        if(num3==1)                                                                                                                                                                                                                                                            
        {  
          turnL(speed);
          delay(200);//如果num2==1说明还没有转到中间位置，继续左转
        }
        else
          break; 
          }   
  
  }
  }
    if(dir==1)
  {
  findline(speed);
  if((num1==0)&&(num5==0))
  { 
    forward(speed);
    delay(200);
    turnR(speed);
    delay(1000);
   // stop();
    while(1){
        num3=digitalRead(11);  //循环判断中间传感器度数，
        if(num3==1)                                                                                                                                                                                                                                                            
        {  
          turnR(speed);
          delay(200);//如果num2==1说明还没有转到中间位置，继续左转
        }
        else
          break; 
          }   
  
  }
  }
}
void bizhang()
{
  if(a<10 and Ldec==0 and Rdec==0)
{
  stop();
  delay(100);
  tData[0] = 30;
  tData[1] = (100/20);
  motor.Servo_Tx(0xFA,12,4,0x01,tData);
  delay(1000);
  b=sr04.Distance();
  delay(10);
  if(b<10)
    Ldec=1;
  delay(1100);  
  tData[0] = 215;
  tData[1] = (100/20);
  motor.Servo_Tx(0xFA,12,4,0x01,tData);
  delay(1000);
  c=sr04.Distance();
  delay(20);
   if(c<10)
    Rdec=1;
  tData[0] = 120;
  tData[1] = (100/20);
  motor.Servo_Tx(0xFA,12,4,0x01,tData);
  delay(1000);
}
else if(Ldec==1 and Rdec==0)
{
  turnR(760);
  delay(2000);
  forward(0);
  //Ldec=0;
}
else if(Rdec==1 and Ldec==0)
{
  turnL(760);
  delay(2000);
  forward(0);
  //Rdec=0;
}
else if(Rdec==1 and Ldec==1)
{
  back(760);
  delay(2000);
 // Ldec=0;
//  Rdec=0;
}
else 
{
  forward(760);

}
 
}
void findline(int i)
{

  num1=digitalRead(31);    
  num2=digitalRead(33);
  num3=digitalRead(35);
  num4=digitalRead(37);
  num5=digitalRead(39);   
 
  if((num2==0)&&num3&&num4) 
   {
      turnL(i);          //检测到左边传感器遇到黑线，说明小车右偏了，立即往左转一点
      delay(40);
      while(1){
        num3=digitalRead(11);  //循环判断中间传感器度数，
        if(num3==1)
        {  
          turnL(i);
          delay(200);//如果num2==1说明还没有转到中间位置，继续左转
        }
        else
          break; 
       
          }                                      
   }    
    
  else if(num3&&num2&&(num4==0))
   {
      turnR(i);
      delay(40);
      while(1)
      {
        num3=digitalRead(11);
        if(num3==1)
        {
           turnR(i);  
           delay(120); 

        }
        else
            break;
        
      }
   }
  
  else
  {
    forward(i); 
    delay(2);
  } 
  }
void forward(int speed)
{
  motor.motor(4,0,speed);
  motor.motor(2,1,speed);
}
void turnL(int speed)
{
  motor.motor(4,1,speed);
  motor.motor(2,1,speed);
  

}
void turnR(int speed)
{

  motor.motor(2,0,speed);
  motor.motor(4,0,speed);

}
void back(int speed)
{
  motor.motor(4,1,speed);
  motor.motor(2,0,speed);
}
void stop()
{
  motor.motor(4,0,0);
  motor.motor(2,0,0);
}

