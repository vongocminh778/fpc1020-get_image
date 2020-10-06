#include <SoftwareSerial.h>

SoftwareSerial *mySerial = NULL;
SoftwareSerial swSerial(13, 15); // RX, TX

#define UART_BUF_LEN  8
#define BUF_N   8
uint8_t IMG[26000];       //Image side 192x185
unsigned char tBuf[UART_BUF_LEN];  //Send commands or data
unsigned char g_ucUartRxEnd ;      //Receive return data end flag
unsigned char g_ucUartRxLen ;      //Receive return data length

unsigned char CmdGenCHK();
void UART_SendPackage();
unsigned char Wait_Command(); 
unsigned char Wait_Command1(); 

void FPC1020(SoftwareSerial *ser) 
  {
    mySerial = ser; // ...override gpsHwSerial with value passed.
    mySerial->begin(9600);
  }


void setup() {
  Serial.begin(57600);
  FPC1020(&swSerial);
  Serial.println("Starting now..!");
}

void loop() {
  // put your main code here, to run repeatedly:
//  unsigned long timeBegin = micros();
  Wait_Command();
//  unsigned long timeEnd = micros();
//  unsigned long duration = timeEnd - timeBegin;
//  double averageDuration = (double)duration / 1000.0;
//  Serial.println(averageDuration);
//  for (int i = 0; i<100; i++)
//  {
//    if (i%100 == 0)
//    Serial.println();
//    Serial.print(IMG[i],HEX);
//    Serial.print(", ");
//  }
//      for (uint8_t j = 0; j < myBuffer.size(); j++) {
//        if (j%100 == 0)
//        Serial.println();
//        Serial.print(myBuffer[j],HEX);
//        Serial.print(", ");
//        delay(10);
//  }
}

// check sum
unsigned char CmdGenCHK(unsigned char wLen,unsigned char *ptr)
{
  unsigned char i,temp = 0;
  
  for(i = 0; i < wLen; i++)
  {
    temp ^= *(ptr + i);
  }
  return temp;
}

// Send command
void UART_SendPackage(unsigned char wLen,unsigned char *ptr)
{
  unsigned int i=0,len=0;
 
  tBuf[0] = 0xf5;     //command head
  for(i = 0; i < wLen; i++)      // data in packet 
  {
    tBuf[1+i] = *(ptr+i);
  } 
  
  tBuf[wLen + 1] = CmdGenCHK(wLen, ptr);         //Generate checkout data
  tBuf[wLen + 2] = 0xf5;
  len = wLen + 3;
  
  g_ucUartRxEnd = 0;
  g_ucUartRxLen = len ;
  
  mySerial->write(tBuf,len); 

 // Serial.print((char *)tBuf);     
//  UartSend(tBuf,len);
}

unsigned char Wait_Command(void)
{
    unsigned char buf[BUF_N];
    long rBuf_p=0;
    int sum =0;
    *buf = 0x24; // 0x24        
    *(buf+1) = 0x00;
    *(buf+2) = 0x00;
    *(buf+3) = 0x20;
    *(buf+4) = 0x00;
    UART_SendPackage(5, buf);
    while(mySerial->available()<= 0);
    for(int i=100; i>0; i--)
    {
//        delay(10);
        delayMicroseconds(800);
        IMG[rBuf_p++] = mySerial->read();
        Serial.print(IMG[rBuf_p],HEX);
        Serial.print(", ");
        if(mySerial->available() == 0)
        {
            break;
        }
//        if (mySerial->overflow()){
//    Serial.println("It overflowed!");
//  }
    }
//    Serial.print(rBuf_p); Serial.println(" bytes read.");  
  Serial.println("");  
}

unsigned char Wait_Command1(void)
{
    uint8_t idx;
    uint16_t timer=0;

    idx = 0;
    
    unsigned char buf[BUF_N];
    long rBuf_p=0;
    *buf = 0x24; // 0x24        
    *(buf+1) = 0x00;
    *(buf+2) = 0x00;
    *(buf+3) = 0x20;
    *(buf+4) = 0x00;
    UART_SendPackage(5, buf);
    while (!mySerial->available()) {
      delay(1);
    }
    // something to read!
    IMG[idx] = mySerial->read();
    Serial.print(idx);Serial.print(",");
}
