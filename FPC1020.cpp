#include "FPC1020.h"
#include "Arduino.h" 

#define UART_BUF_LEN  8
#define BUF_N   8
#include <iostream> 

unsigned char rBuf[192];           //Receive return data
uint8_t IMG[4000];       //Image side 192x185
unsigned char tBuf[UART_BUF_LEN];  //Send commands or data
unsigned char g_ucUartRxEnd ;      //Receive return data end flag
unsigned char g_ucUartRxLen ;      //Receive return data length
unsigned char l_ucFPID =1;         //User ID
String stringOne;

SoftwareSerial *mySerial = NULL;

FPC1020::FPC1020(SoftwareSerial *ser) {
  mySerial = ser; // ...override gpsHwSerial with value passed.
  mySerial->begin(19200);
}

//Function：wait data packet send finish
unsigned char FPC1020::WaitFpData(void)
{
    int i, temp = 0;
    unsigned char rBuf_p = 0;
    
//    while(mySerial->available()<= 0)
//    {
//      if (WiFi.status() == WL_CONNECTED)
//        digitalWrite(4, HIGH);
//      else
//      { 
//        digitalWrite(4, LOW);
//        return FALSE;
//        temp = 1;
//        break;
//      }
//    }
//    digitalWrite(0, HIGH);
    for(i=200; i>0; i--)//wait response info
    {
        delay(20);
        rBuf[rBuf_p++] = mySerial->read();
        
        if(mySerial->available() == 0)
        {
            g_ucUartRxEnd = 1;
            g_ucUartRxLen = rBuf_p;
            break;
        }
    }
    
    if(rBuf_p == 0 || temp == 1) return FALSE;
    else return TRUE;
}

// check sum
unsigned char FPC1020::CmdGenCHK(unsigned char wLen,unsigned char *ptr)
{
  unsigned char i,temp = 0;
  
  for(i = 0; i < wLen; i++)
  {
    temp ^= *(ptr + i);
  }
  return temp;
}

// Send command
void FPC1020::UART_SendPackage(unsigned char wLen,unsigned char *ptr)
{
  unsigned int i=0,len=0;
 
  tBuf[0] = DATA_START;     //command head
  for(i = 0; i < wLen; i++)      // data in packet 
  {
    tBuf[1+i] = *(ptr+i);
  } 
  
  tBuf[wLen + 1] = CmdGenCHK(wLen, ptr);         //Generate checkout data
  tBuf[wLen + 2] = DATA_END;
  len = wLen + 3;
  
  g_ucUartRxEnd = 0;
  g_ucUartRxLen = len ;
  
  mySerial->write(tBuf,len); 

 // Serial.print((char *)tBuf);     
//  UartSend(tBuf,len);
}

void FPC1020::sent_data_img()
{
  unsigned char buf[BUF_N];
    *buf = 0x24; // 0x24        
    *(buf+1) = 0x00;
    *(buf+2) = 0x00;
    *(buf+3) = 0x00;
    *(buf+4) = 0x00;

  UART_SendPackage(5, buf);
}

unsigned char FPC1020::WaitFpData_Image(void)
{
    unsigned char buf[BUF_N];
    long rBuf_p = 0;
    
 
    sent_data_img();
    while(mySerial->available()<= 0);
    for(int i=5000; i>0; i--)
    {
//        delay(20);
        delayMicroseconds(1000);
        IMG[rBuf_p++] = mySerial->read();    
        if(mySerial->available() == 0)
        {
            g_ucUartRxEnd = 1;
            g_ucUartRxLen = rBuf_p;
            Serial.print("g_ucUartRxLen:");     
            Serial.println(g_ucUartRxLen);   
            Serial.println();    
            break;
        }
    }
    if(rBuf_p == 0) return FALSE;
    else return TRUE;
}


unsigned char FPC1020::Wait_Command(void)
{
    unsigned char buf[BUF_N];
    long rBuf_p=0;
    *buf = 0x24; // 0x24        
    *(buf+1) = 0x00;
    *(buf+2) = 0x00;
    *(buf+3) = 0x20;
    *(buf+4) = 0x00;
    UART_SendPackage(5, buf);
    while(mySerial->available()<= 0);
    for(int i=3300; i>0; i--)
    {
//        delay(20);
        delayMicroseconds(555);
        IMG[rBuf_p++] = mySerial->read();
        if(mySerial->available() == 0)
        {
//            g_ucUartRxEnd = 1;
//            g_ucUartRxLen = rBuf_p;
            break;
        }
    }
    Serial.print(rBuf_p); Serial.println(" bytes read.");  
    if(rBuf_p == 0) return FALSE;
    else return TRUE;
}
