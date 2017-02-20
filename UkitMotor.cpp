#include"Arduino.h"  
#include"UkitMotor.h"  
UkitMotor::UkitMotor()
{
;
}
UkitMotor::~UkitMotor()
{
  ;
}

unsigned char UkitMotor::Cheak_Sum(unsigned char len, unsigned char *buf)
{
  unsigned char i, sum = 0;
  
  if(len > 254) return 0;
  for(i = 0; i < len; i++)
  {
    sum += buf[i];
  }
  return  (uint8_t)(sum);
}
  
unsigned long UkitMotor::Servo_Tx(unsigned char Head,unsigned char ServoNO,unsigned char len,unsigned char CMD,unsigned char * Data)
{
  unsigned long tRet = 0;
  unsigned char tCnt = 0;
  unsigned long temp = 2; //2ms 发完
  unsigned char buf[40];
  unsigned char length = 9; //9+1

  unsigned char Usart3_Rx_Ack_Len=0;
  
  memset((void *)Usart3_Rx_Buf,0,sizeof(Usart3_Rx_Buf));
  memset((void *)buf,0,sizeof(buf));
  Usart3_Rx_Ack_Len = 10; //应答消息长度
  
  buf[0] = Head;  //填充协议头
  buf[1] = swab8(Head);
  if(Head >= 0xFA)
  {
    buf[2] = ServoNO; //舵机好
  }
  else if(Head > 0x00)  //变长协议
  {
    length = len + 5; //长度
    buf[2] = length;
  }
  buf[3] = CMD;
  memcpy((void *)&buf[4],(void *)Data,len);
  buf[length - 1] = Cheak_Sum( (length - 3),(u8*)&buf[2]);
  buf[length ] = 0xED;
  
  
  if(Head == 0xFA)
  {
    if(CMD == 1 )
    {
      if(ServoNO == 0)
      {
        //这句有问题 memset 不支持双字节的填充
        //memset((void *)&gsSave_Angle,0xFF,sizeof(gsSave_Angle));  //保存所有角度
        //memset((void *)&gsSave_Angle,Data,sizeof(gsSave_Angle));  //保存所有角度
        memset((void *)&gsSave_Angle,Data[0],sizeof(gsSave_Angle)); //保存所有角度
      }
      else
      {
        if(Data[0] == gsSave_Angle.Angle[ServoNO - 1])  return  0;  //不发送,直接返回
        else
        {
          gsSave_Angle.Angle[ServoNO - 1] = Data[0];  //保存单个角度,以后可能双字节角度
        }
      }
      Usart3_Rx_Ack_Len = 1;  //1,4命令只应答一个字节
    }
    else if( CMD == 4)  Usart3_Rx_Ack_Len = 1;  //1,4命令只应答一个字节
  }
  
Retry_Servo:
  
  temp = (Usart3_Rx_Ack_Len + 5) ;  //接收消息长度,用于计算接收时间,1个字节 0.087ms,预留5个空闲,10%误差
  Serial3.begin(115200);  //uart3
  Serial3.setTimeout(temp*87*110/100 / 400);  //设置超时ms
  Serial2.begin(115200);  //设置波特率
  Serial2.write(buf,length + 1);  //发送消息
  Serial2.end();  //关闭串口2,否则会影响接收消息
  
  tRet = Serial3.readBytes( Usart3_Rx_Buf, Usart3_Rx_Ack_Len+10); //接收应答
  Serial3.end();  //关闭串口3,否则会影响接收消息
  if(tRet > RXD_OFFSET) //打印返回消息
  {
      //Serial.write(Usart3_Rx_Buf + RXD_OFFSET,tRet - RXD_OFFSET);
      //Serial.print("\r\n");
  }
  
  if(tRet == 0) //没有接收到消息
  {
    if( tCnt < 2)
    {
      tCnt ++;  //重试
      goto  Retry_Servo;
    }
  }
  else  //接收到消息
  {
  Usart3_Rx_Buf_count = tRet;
    tRet = 0;
    if(Head == 0xFA)
    {
      if( (CMD == 1) || (CMD == 4) )
      {
        if( (Usart3_Rx_Buf[RXD_OFFSET + 1] == 0xAA + ServoNO) )
        {
          tRet = ServoNO;
        }
      }
      else if(CMD == 2 )
      {
        if( (Usart3_Rx_Buf[RXD_OFFSET + 3] == 0xAA) && (Usart3_Rx_Buf[RXD_OFFSET + 2] == ServoNO) )
        {
          tRet = Usart3_Rx_Buf[10+6] << 8;
          tRet |= Usart3_Rx_Buf[10+7] << 0;
        }
      }
      else if(CMD == 0xCD )
      {
        if( (Usart3_Rx_Buf[RXD_OFFSET + 5] == ServoNO) )
        {
          tRet = Usart3_Rx_Buf[RXD_OFFSET + 5];
        }
      }
      else if(CMD == 0xD2 )
      {
        if( (Usart3_Rx_Buf[RXD_OFFSET + 3] == 0xAA) && (Usart3_Rx_Buf[RXD_OFFSET + 2] == ServoNO) )
        {
          tRet = 1;
        }
      }
      else if(CMD == 0xD4 )
      {
        if( (Usart3_Rx_Buf[RXD_OFFSET + 3] == 0xAA) && (Usart3_Rx_Buf[RXD_OFFSET + 2] == ServoNO) )
        {
          tRet = Usart3_Rx_Buf[RXD_OFFSET + 6] << 8;
          tRet |= Usart3_Rx_Buf[RXD_OFFSET + 7] << 0;
        }
      }
      
    }
    else if(Head == 0xFC)
    {
      if(CMD == 1 )
      {
        if((Usart3_Rx_Buf[RXD_OFFSET + 3] == 0xAA) && (Usart3_Rx_Buf[RXD_OFFSET + 2] == ServoNO) )
        {
          tRet = Usart3_Rx_Buf[RXD_OFFSET + 6] << 8;
          tRet |= Usart3_Rx_Buf[RXD_OFFSET + 7] << 0;
        }
      }
    }
    else if( (Head < 0xFA) && (Head > 0x00) && (Usart3_Rx_Buf[length + 4] == CMD) )
    {
      if(CMD == 0x02 )
      {
        tRet = Usart3_Rx_Buf[length + 5] - 0xAA;
      }
      else if(CMD == 0x04 )
      {
  if(Usart3_Rx_Buf[length + 3] == 7)
        tRet = ((Usart3_Rx_Buf[length + 5] - 0xAA) << 8) + Usart3_Rx_Buf[length + 6];
else   if(Usart3_Rx_Buf[length + 3] == 8)
  tRet = (Usart3_Rx_Buf[length + 6] << 8) + Usart3_Rx_Buf[length + 7];
      }
      
    }
  }
  return tRet;
}

unsigned char UkitMotor::Get_Servo_list(unsigned char Servo_NO)  //检查舵机存在不 ? 正不正常
{
  return(Read_bit(gServo_list[Servo_NO / 8],Servo_NO % 8 ));
}
void UkitMotor:: Set_Servo_list(unsigned char Servo_NO)
{
  Set_bit(gServo_list[Servo_NO / 8],Servo_NO % 8 );
}
void UkitMotor::Clr_Servo_list(unsigned char Servo_NO)
{
  Clr_bit(gServo_list[Servo_NO / 8],Servo_NO % 8 );
}


void UkitMotor::check_servo()
{//花两秒时间
  unsigned char tID = 1;
  unsigned char tData[4] = {0,0,0,0};
  
  memset((void*)gServo_list,0,sizeof(gServo_list));
  gServos = 0;
  Servo_Tx(0xFC,tID,4,0x01,tData ); //预先测试一下
  for(tID = 1; tID < SERVO_NUMER_MAX + 1; tID++)
  {
    if( Servo_Tx(0xFC,tID,4,0x01,tData ) != 0 )
    {
      Set_Servo_list(tID - 1);  //设置舵机列表,后面多舵机使用
      gServos++;
    }
    else
    {
      Clr_Servo_list(tID - 1);
    }
  }
  Serial.print("\tTotal Servo:");
  Serial.print(gServos, DEC);
  Serial.print("\r\n");
}
void UkitMotor::motor(char ID,int dir, int speed)
{
   unsigned char aa[4]={0xFE,0,0,speed};
   unsigned char bb[4]={0xFD,0,0,speed};
  if(dir==0)
       Servo_Tx(0xFA,ID,4,0x01,aa); 
    else
       Servo_Tx(0xFA,ID,4,0x01,bb); 
   

}
void UkitMotor::servo(char ID,int angle,int time)  //<ANGLE>:1~180  <TIME>:20~5000
{
  Servo_Tx(0xFA,ID,4,0x01,angle<<0 | time<<8 );
}

