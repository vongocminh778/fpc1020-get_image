//#include <SoftwareSerial.h>
#include "FS.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED true

#define RXD2 16
#define TXD2 17
#define UART_BUF_LEN  8
#define BUF_N   8

// Declare variable
char buffer[3300]= {};
uint8_t IMG[40000];       //Image side 192x185
unsigned char tBuf[UART_BUF_LEN];  //Send commands or data
unsigned char g_ucUartRxEnd ;      //Receive return data end flag
unsigned char g_ucUartRxLen ;      //Receive return data length


// Declare the function
unsigned char CmdGenCHK();
void UART_SendPackage();
unsigned char Wait_Command();
void listDir(); 
void readFile();
void writeFile();
void appendFile();
void renameFile();
void deleteFile();
void testFileIO();
void readData();

void setup() {
  int xCnt = 0;
  Serial.begin(19200);
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);
//  SPIFFS.begin();
//  File f = SPIFFS.open("/data.txt", "r");
//  if (!f) {
//      Serial.println("File open failed");
//    } else {
//
//      //we're only tracking the first 100 lines, so we don't care if there is anything after that.
//      while(f.available() && xCnt < 100) {
//        //Lets read line by line from the file
//        String line = f.readStringUntil('\n');
//        IMG[xCnt] = line.toInt();
//        Serial.print(xCnt);
//        Serial.print("  ");
//        Serial.println(line);
//        xCnt ++;
//      } 
//    }
//    f.close();
  
//  listDir(SPIFFS, "/", 0);
  Serial.println("Starting now..!");
}

void loop() {
  // put your main code here, to run repeatedly:
    Wait_Command();
//    readData();
  
  for (int i = 0; i<3300; i++)
  {
    if (i%100 == 0)
      Serial.println();
    Serial.print(IMG[i],HEX);
    Serial.print(", ");
  }
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
  
  Serial2.write(tBuf,len); 

 // Serial.print((char *)tBuf);     
//  UartSend(tBuf,len);
}

unsigned char Wait_Command(void)
{
    unsigned char buf[BUF_N];
    long rBuf_p=0;
//    File f = SPIFFS.open("/data.txt",FILE_APPEND);
    *buf = 0x24; // 0x24        
    *(buf+1) = 0x00;
    *(buf+2) = 0x00;
    *(buf+3) = 0x00;
    *(buf+4) = 0x00;
    UART_SendPackage(5, buf);
    while(Serial2.available()<= 0);
    for(int i=3300; i>0; i--)
    {
//        delay(5);
        delayMicroseconds(555);
        IMG[rBuf_p++] = Serial2.read();
//          IMG[rBuf_p++] = Serial2.readBytes(buffer,3);
//        f.println(IMG[rBuf_p]);
        if(Serial2.available() == 0)
        {
//            g_ucUartRxEnd = 1;
//            g_ucUartRxLen = rBuf_p;
            break;
        }
//        Serial.println(buffer);
    }
//    f.close();
//    Serial.print(rBuf_p); Serial.println(" bytes read.");  
}

void readData(){
  
  int xCnt = 0;
  
  File f = SPIFFS.open("/data.txt", "r");
  
  if (!f) {
      Serial.println("file open failed");
  }  Serial.println("====== Reading from SPIFFS file =======");

    while(f.available()) {
      //Lets read line by line from the file
      String line = f.readStringUntil('\n');
      Serial.print(xCnt);
      Serial.print("  ");
      Serial.println(line);
      xCnt ++;
    }
    f.close();    

}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, uint8_t message){
//    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
//        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
//        Serial.println("- file written");
    } else {
//        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, uint8_t message){
//    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
//        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
//        Serial.println("- message appended");
    } else {
//        Serial.println("- append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("- failed to open file for reading");
    }
}
