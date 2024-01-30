#include <SPI.h>
#include <stdint.h>

int dt = 200;
float CLK_FREQ = 8*pow(10,6);//CLOCK frequency (default 8MHz)
float CLK_PERIOD = 1/CLK_FREQ;
uint32_t SPIfrequency = 20000000;

//*******************************TDC7200 REGISTER ADDRESS LIST*****************************

byte CONFIG1 = 0x00;
byte CONFIG2 = 0x01;
byte INT_STATUS =0x02;
byte INT_MASK = 0x03;
byte COARSE_CNTR_OVF_H = 0x04;
byte COARSE_CNTR_OVF_L = 0x05;
byte CLOCK_CNTR_OVF_H = 0x06;
byte CLOCK_CNTR_OVF_L = 0x07;
byte CLOCK_CNTR_STOP_MASK_H = 0x08;
byte CLOCK_CNTR_STOP_MASK_L = 0x09;

byte TIME1 = 0x10;
byte CLOCK_COUNT1 = 0x11;
byte TIME2 = 0x12;
byte CALIBRATION1 = 0x1B;
byte CALIBRATION2 = 0x1C;
//***************************END OF TDC7200 REGISTER ADDRESS LIST**************************

int selectTDC1000 = 5;
int enableTDC1000 = 7;
int dataReadyPin = 4;
int channelSelect = 3;

int trigger = 2; 
int start = A5; 


void setup() {
   Serial.begin(115200 );
  Serial.println("----------------------Program Start-------------------------");
  SPI.begin();
  pinMode(selectTDC1000, OUTPUT);
  pinMode(enableTDC1000, OUTPUT);
  pinMode(dataReadyPin, INPUT); //Unused in loops
  pinMode(channelSelect, OUTPUT); //Unused in loops
  delay(dt);
  pinMode(trigger, OUTPUT); 
  pinMode(start, INPUT); 

  digitalWrite(trigger, LOW); 

TDC1000Write(CONFIG1,0x02);
TDC1000Write(CONFIG2,0x00);
TDC1000Write(INT_STATUS,0x06);
TDC1000Write(INT_MASK,0x89);

uint16_t readCONFIG1 = TDC1000Read(CONFIG1);
uint16_t readCONFIG2 = TDC1000Read(CONFIG2);
uint16_t readINT_STATUS = TDC1000Read(INT_STATUS);
uint16_t readINT_MASK = TDC1000Read(INT_MASK);

  Serial.print("CONFIG1= ");
  Serial.println(readCONFIG1,BIN);
  Serial.print("\n");
  Serial.print("CONFIG2= ");
  Serial.println(readCONFIG2,BIN);
  Serial.print("\n");
  Serial.print("INT_STATUS= ");
  Serial.println(readINT_STATUS,BIN);
  Serial.print("\n");
  Serial.print("INT_MASK= ");
  Serial.println(readINT_MASK,BIN);
  Serial.print("\n");

  digitalWrite(enableTDC1000, HIGH);
  
}

static volatile uint16_t tstart_us;
static volatile uint16_t tstop_us;

void loop() {
  // put your main code here, to run repeatedly:
  SPI.beginTransaction(SPISettings(SPIfrequency,MSBFIRST,SPI_MODE3));

  tstart_us = 0u;
  tstop_us  = 0u;

    // Trigger new measurement
    digitalWrite(trigger, HIGH);
    digitalWrite(trigger, LOW);

    bool timeout = false;
    unsigned long tstart = micros();
    for (;;)
    {
        if (tstop_us) break;
        if (micros() - tstart > 5000u)
        {
            timeout = true;
            break;
        }
    }
    Serial.println(tstop_us - tstart_us);
    delay(1000); 
}

void TDC1000Write(byte address,byte data){
  SPI.beginTransaction(SPISettings(SPIfrequency,MSBFIRST,SPI_MODE3));
  digitalWrite(selectTDC1000, LOW);//between selectTDC1000 OR selectTDC7200
  delayMicroseconds(100);
  address |= 0x40;
  SPI.transfer(address);
  SPI.transfer(data);
  digitalWrite(selectTDC1000, HIGH);
  SPI.endTransaction();
  };


byte TDC1000Read(byte address){
  SPI.beginTransaction(SPISettings(SPIfrequency,MSBFIRST,SPI_MODE3));
  digitalWrite(selectTDC1000, LOW);
  delayMicroseconds(100);
  //SPI.transfer(address);
  SPI.transfer(address);
  byte inByte = SPI.transfer(0x00);
  digitalWrite(selectTDC1000, HIGH);
  SPI.endTransaction();
  return inByte;
  };