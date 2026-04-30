/*
  ELECHOUSE_CC1101_2.cpp - CC1101 module library
  Copyright (c) 2010 Michael.
    Author: Michael, <www.elechouse.com>
    Version: November 12, 2010

  This library is designed to use CC1101/CC1100 module on Arduino platform.
  CC1101/CC1100 module is an useful wireless module.Using the functions of the 
  library, you can easily send and receive data by the CC1101/CC1100 module. 
  Just have fun!
  For the details, please refer to the datasheet of CC1100/CC1101.
----------------------------------------------------------------------------------------------------------------
cc1101 Driver for RC Switch. Mod by Little Satan. With permission to modify and publish Wilson Shen (ELECHOUSE).
----------------------------------------------------------------------------------------------------------------
*/
#include <SPI.h>
#include "ELECHOUSE_CC1101_SRC_DRV2.h"
#include <Arduino.h>

/****************************************************************/
#define   WRITE_BURST       0x40            //write burst
#define   READ_SINGLE       0x80            //read single
#define   READ_BURST        0xC0            //read burst
#define   BYTES_IN_RXFIFO   0x7F            //byte number in RXfifo
#define   max_modul 6

byte modulation_2 = 2;
byte frend0_2;
byte chan_2 = 0;
int pa_2 = 12;
byte last_pa_2;
byte SCK_PIN_2;
byte MISO_PIN_2;
byte MOSI_PIN_2;
byte SS_PIN_2;
byte GDO0_2;
byte GDO2_2;
byte SCK_PIN_M_2[max_modul];
byte MISO_PIN_M_2[max_modul];
byte MOSI_PIN_M_2[max_modul];
byte SS_PIN_M_2[max_modul];
byte GDO0_M_2[max_modul];
byte GDO2_M_2[max_modul];
byte gdo_set_2=0;
bool spi_2 = 0;
bool ccmode_2 = 0;
float MHz_2 = 433.92;
byte m4RxBw_2 = 0;
byte m4DaRa_2;
byte m2DCOFF_2;
byte m2MODFM_2;
byte m2MANCH_2;
byte m2SYNCM_2;
byte m1FEC_2;
byte m1PRE_2;
byte m1CHSP_2;
byte pc1PQT_2;
byte pc1CRC_AF_2;
byte pc1APP_ST_2;
byte pc1ADRCHK_2;
byte pc0WDATA_2;
byte pc0PktForm_2;
byte pc0CRC_EN_2;
byte pc0LenConf_2;
byte trxstate_2 = 0;
byte clb1_2[2]= {24,28};
byte clb2_2[2]= {31,38};
byte clb3_2[2]= {65,76};
byte clb4_2[2]= {77,79};

/****************************************************************/
uint8_t PA_TABLE_2[8]     {0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00};
//                       -30  -20  -15  -10   0    5    7    10
uint8_t PA_TABLE_315_2[8] {0x12,0x0D,0x1C,0x34,0x51,0x85,0xCB,0xC2,};             //300 - 348
uint8_t PA_TABLE_433_2[8] {0x12,0x0E,0x1D,0x34,0x60,0x84,0xC8,0xC0,};             //387 - 464
//                        -30  -20  -15  -10  -6    0    5    7    10   12
uint8_t PA_TABLE_868_2[10] {0x03,0x17,0x1D,0x26,0x37,0x50,0x86,0xCD,0xC5,0xC0,};  //779 - 899.99
//                        -30  -20  -15  -10  -6    0    5    7    10   11
uint8_t PA_TABLE_915_2[10] {0x03,0x0E,0x1E,0x27,0x38,0x8E,0x84,0xCC,0xC3,0xC0,};  //900 - 928
/****************************************************************
*FUNCTION NAME:SpiStart
*FUNCTION     :spi_2 communication start
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SpiStart(void)
{
  // initialize the SPI pins
  pinMode(SCK_PIN_2, OUTPUT);
  pinMode(MOSI_PIN_2, OUTPUT);
  pinMode(MISO_PIN_2, INPUT);
  pinMode(SS_PIN_2, OUTPUT);

  // enable SPI
  #ifdef ESP32
  SPI.begin(SCK_PIN_2, MISO_PIN_2, MOSI_PIN_2, SS_PIN_2);
  #else
  SPI.begin();
  #endif
}
/****************************************************************
*FUNCTION NAME:SpiEnd
*FUNCTION     :spi_2 communication disable
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SpiEnd(void)
{
  // disable SPI
  SPI.endTransaction();
  SPI.end();
}
/****************************************************************
*FUNCTION NAME: GDO_Set()
*FUNCTION     : set GDO0_2,GDO2_2 pin for serial pinmode.
*INPUT        : none
*OUTPUT       : none
****************************************************************/
void ELECHOUSE_CC1101_2::GDO_Set (void)
{
	pinMode(GDO0_2, INPUT);
	pinMode(GDO2_2, INPUT);
}
/****************************************************************
*FUNCTION NAME: GDO_Set()
*FUNCTION     : set GDO0_2 for internal transmission mode.
*INPUT        : none
*OUTPUT       : none
****************************************************************/
void ELECHOUSE_CC1101_2::GDO0_Set (void)
{
  pinMode(GDO0_2, INPUT);
}
/****************************************************************
*FUNCTION NAME:Reset
*FUNCTION     :CC1101 reset //details refer datasheet of CC1101/CC1100//
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::Reset (void)
{
	digitalWrite(SS_PIN_2, LOW);
	delay(1);
	digitalWrite(SS_PIN_2, HIGH);
	delay(1);
	digitalWrite(SS_PIN_2, LOW);
	while(digitalRead(MISO_PIN_2));
  SPI.transfer(CC1101_SRES);
  while(digitalRead(MISO_PIN_2));
	digitalWrite(SS_PIN_2, HIGH);
}
/****************************************************************
*FUNCTION NAME:Init
*FUNCTION     :CC1101 initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::Init(void)
{
  setSpi();
  SpiStart();                   //spi_2 initialization
  digitalWrite(SS_PIN_2, HIGH);
  digitalWrite(SCK_PIN_2, HIGH);
  digitalWrite(MOSI_PIN_2, LOW);
  Reset();                    //CC1101 reset
  RegConfigSettings();            //CC1101 register config
  SpiEnd();
}
/****************************************************************
*FUNCTION NAME:SpiWriteReg
*FUNCTION     :CC1101 write data to register
*INPUT        :addr: register address; value: register value
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SpiWriteReg(byte addr, byte value)
{
  SpiStart();
  digitalWrite(SS_PIN_2, LOW);
  while(digitalRead(MISO_PIN_2));
  SPI.transfer(addr);
  SPI.transfer(value); 
  digitalWrite(SS_PIN_2, HIGH);
  SpiEnd();
}
/****************************************************************
*FUNCTION NAME:SpiWriteBurstReg
*FUNCTION     :CC1101 write burst data to register
*INPUT        :addr: register address; buffer:register value array; num:number to write
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SpiWriteBurstReg(byte addr, byte *buffer, byte num)
{
  byte i, temp;
  SpiStart();
  temp = addr | WRITE_BURST;
  digitalWrite(SS_PIN_2, LOW);
  while(digitalRead(MISO_PIN_2));
  SPI.transfer(temp);
  for (i = 0; i < num; i++)
  {
  SPI.transfer(buffer[i]);
  }
  digitalWrite(SS_PIN_2, HIGH);
  SpiEnd();
}
/****************************************************************
*FUNCTION NAME:SpiStrobe
*FUNCTION     :CC1101 Strobe
*INPUT        :strobe: command; //refer define in CC1101.h//
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SpiStrobe(byte strobe)
{
  SpiStart();
  digitalWrite(SS_PIN_2, LOW);
  while(digitalRead(MISO_PIN_2));
  SPI.transfer(strobe);
  digitalWrite(SS_PIN_2, HIGH);
  SpiEnd();
}
/****************************************************************
*FUNCTION NAME:SpiReadReg
*FUNCTION     :CC1101 read data from register
*INPUT        :addr: register address
*OUTPUT       :register value
****************************************************************/
byte ELECHOUSE_CC1101_2::SpiReadReg(byte addr) 
{
  byte temp, value;
  SpiStart();
  temp = addr| READ_SINGLE;
  digitalWrite(SS_PIN_2, LOW);
  while(digitalRead(MISO_PIN_2));
  SPI.transfer(temp);
  value=SPI.transfer(0);
  digitalWrite(SS_PIN_2, HIGH);
  SpiEnd();
  return value;
}

/****************************************************************
*FUNCTION NAME:SpiReadBurstReg
*FUNCTION     :CC1101 read burst data from register
*INPUT        :addr: register address; buffer:array to store register value; num: number to read
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SpiReadBurstReg(byte addr, byte *buffer, byte num)
{
  byte i,temp;
  SpiStart();
  temp = addr | READ_BURST;
  digitalWrite(SS_PIN_2, LOW);
  while(digitalRead(MISO_PIN_2));
  SPI.transfer(temp);
  for(i=0;i<num;i++)
  {
  buffer[i]=SPI.transfer(0);
  }
  digitalWrite(SS_PIN_2, HIGH);
  SpiEnd();
}

/****************************************************************
*FUNCTION NAME:SpiReadStatus
*FUNCTION     :CC1101 read status register
*INPUT        :addr: register address
*OUTPUT       :status value
****************************************************************/
byte ELECHOUSE_CC1101_2::SpiReadStatus(byte addr) 
{
  byte value,temp;
  SpiStart();
  temp = addr | READ_BURST;
  digitalWrite(SS_PIN_2, LOW);
  while(digitalRead(MISO_PIN_2));
  SPI.transfer(temp);
  value=SPI.transfer(0);
  digitalWrite(SS_PIN_2, HIGH);
  SpiEnd();
  return value;
}
/****************************************************************
*FUNCTION NAME:SPI pin Settings
*FUNCTION     :Set Spi pins
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setSpi(void){
  if (spi_2 == 0){
  #if defined __AVR_ATmega168__ || defined __AVR_ATmega328P__
  SCK_PIN_2 = 13; MISO_PIN_2 = 12; MOSI_PIN_2 = 11; SS_PIN_2 = 10;
  #elif defined __AVR_ATmega1280__ || defined __AVR_ATmega2560__
  SCK_PIN_2 = 52; MISO_PIN_2 = 50; MOSI_PIN_2 = 51; SS_PIN_2 = 53;
  #elif ESP8266
  SCK_PIN_2 = 14; MISO_PIN_2 = 12; MOSI_PIN_2 = 13; SS_PIN_2 = 15;
  #elif ESP32
  SCK_PIN_2 = 18; MISO_PIN_2 = 19; MOSI_PIN_2 = 23; SS_PIN_2 = 5;
  #else
  SCK_PIN_2 = 18; MISO_PIN_2 = 19; MOSI_PIN_2 = 23; SS_PIN_2 = 5;
  #endif
}
}
/****************************************************************
*FUNCTION NAME:COSTUM SPI
*FUNCTION     :set costum spi_2 pins.
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setSpiPin(byte sck, byte miso, byte mosi, byte ss){
  spi_2 = 1;
  SCK_PIN_2 = sck;
  MISO_PIN_2 = miso;
  MOSI_PIN_2 = mosi;
  SS_PIN_2 = ss;
}
/****************************************************************
*FUNCTION NAME:COSTUM SPI
*FUNCTION     :set costum spi_2 pins.
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::addSpiPin(byte sck, byte miso, byte mosi, byte ss, byte modul){
  spi_2 = 1;
  SCK_PIN_M_2[modul] = sck;
  MISO_PIN_M_2[modul] = miso;
  MOSI_PIN_M_2[modul] = mosi;
  SS_PIN_M_2[modul] = ss;
}
/****************************************************************
*FUNCTION NAME:GDO Pin settings
*FUNCTION     :set GDO Pins
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setGDO(byte gdo0, byte gdo2){
GDO0_2 = gdo0;
GDO2_2 = gdo2;  
GDO_Set();
}
/****************************************************************
*FUNCTION NAME:GDO0_2 Pin setting
*FUNCTION     :set GDO0_2 Pin
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setGDO0(byte gdo0){
GDO0_2 = gdo0;
GDO0_Set();
}
/****************************************************************
*FUNCTION NAME:GDO Pin settings
*FUNCTION     :add GDO Pins
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::addGDO(byte gdo0, byte gdo2, byte modul){
GDO0_M_2[modul] = gdo0;
GDO2_M_2[modul] = gdo2;  
gdo_set_2=2;
GDO_Set();
}
/****************************************************************
*FUNCTION NAME:add GDO0_2 Pin
*FUNCTION     :add GDO0_2 Pin
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::addGDO0(byte gdo0, byte modul){
GDO0_M_2[modul] = gdo0;
gdo_set_2=1;
GDO0_Set();
}
/****************************************************************
*FUNCTION NAME:set Modul
*FUNCTION     :change modul
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setModul(byte modul){
  SCK_PIN_2 = SCK_PIN_M_2[modul];
  MISO_PIN_2 = MISO_PIN_M_2[modul];
  MOSI_PIN_2 = MOSI_PIN_M_2[modul];
  SS_PIN_2 = SS_PIN_M_2[modul];
  if (gdo_set_2==1){
  GDO0_2 = GDO0_M_2[modul];
  }
  else if (gdo_set_2==2){
  GDO0_2 = GDO0_M_2[modul];
  GDO2_2 = GDO2_M_2[modul];
  }
}
/****************************************************************
*FUNCTION NAME:CCMode
*FUNCTION     :Format of RX and TX data
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setCCMode(bool s){
ccmode_2 = s;
if (ccmode_2 == 1){
SpiWriteReg(CC1101_IOCFG2,      0x0B);
SpiWriteReg(CC1101_IOCFG0,      0x06);
SpiWriteReg(CC1101_PKTCTRL0,    0x05);
SpiWriteReg(CC1101_MDMCFG3,     0xF8);
SpiWriteReg(CC1101_MDMCFG4,11+m4RxBw_2);
}else{
SpiWriteReg(CC1101_IOCFG2,      0x0D);
SpiWriteReg(CC1101_IOCFG0,      0x0D);
SpiWriteReg(CC1101_PKTCTRL0,    0x32);
SpiWriteReg(CC1101_MDMCFG3,     0x93);
SpiWriteReg(CC1101_MDMCFG4, 7+m4RxBw_2);
}
setModulation(modulation_2);
}
/****************************************************************
*FUNCTION NAME:Modulation
*FUNCTION     :set CC1101 Modulation 
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setModulation(byte m){
if (m>4){m=4;}
modulation_2 = m;
Split_MDMCFG2();
switch (m)
{
case 0: m2MODFM_2=0x00; frend0_2=0x10; break; // 2-FSK
case 1: m2MODFM_2=0x10; frend0_2=0x10; break; // GFSK
case 2: m2MODFM_2=0x30; frend0_2=0x11; break; // ASK
case 3: m2MODFM_2=0x40; frend0_2=0x10; break; // 4-FSK
case 4: m2MODFM_2=0x70; frend0_2=0x10; break; // MSK
}
SpiWriteReg(CC1101_MDMCFG2, m2DCOFF_2+m2MODFM_2+m2MANCH_2+m2SYNCM_2);
SpiWriteReg(CC1101_FREND0,   frend0_2);
setPA(pa_2);
}
/****************************************************************
*FUNCTION NAME:PA Power
*FUNCTION     :set CC1101 PA Power 
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setPA(int p)
{
int a;
pa_2 = p;

if (MHz_2 >= 300 && MHz_2 <= 348){
if (pa_2 <= -30){a = PA_TABLE_315_2[0];}
else if (pa_2 > -30 && pa_2 <= -20){a = PA_TABLE_315_2[1];}
else if (pa_2 > -20 && pa_2 <= -15){a = PA_TABLE_315_2[2];}
else if (pa_2 > -15 && pa_2 <= -10){a = PA_TABLE_315_2[3];}
else if (pa_2 > -10 && pa_2 <= 0){a = PA_TABLE_315_2[4];}
else if (pa_2 > 0 && pa_2 <= 5){a = PA_TABLE_315_2[5];}
else if (pa_2 > 5 && pa_2 <= 7){a = PA_TABLE_315_2[6];}
else if (pa_2 > 7){a = PA_TABLE_315_2[7];}
last_pa_2 = 1;
}
else if (MHz_2 >= 378 && MHz_2 <= 464){
if (pa_2 <= -30){a = PA_TABLE_433_2[0];}
else if (pa_2 > -30 && pa_2 <= -20){a = PA_TABLE_433_2[1];}
else if (pa_2 > -20 && pa_2 <= -15){a = PA_TABLE_433_2[2];}
else if (pa_2 > -15 && pa_2 <= -10){a = PA_TABLE_433_2[3];}
else if (pa_2 > -10 && pa_2 <= 0){a = PA_TABLE_433_2[4];}
else if (pa_2 > 0 && pa_2 <= 5){a = PA_TABLE_433_2[5];}
else if (pa_2 > 5 && pa_2 <= 7){a = PA_TABLE_433_2[6];}
else if (pa_2 > 7){a = PA_TABLE_433_2[7];}
last_pa_2 = 2;
}
else if (MHz_2 >= 779 && MHz_2 <= 899.99){
if (pa_2 <= -30){a = PA_TABLE_868_2[0];}
else if (pa_2 > -30 && pa_2 <= -20){a = PA_TABLE_868_2[1];}
else if (pa_2 > -20 && pa_2 <= -15){a = PA_TABLE_868_2[2];}
else if (pa_2 > -15 && pa_2 <= -10){a = PA_TABLE_868_2[3];}
else if (pa_2 > -10 && pa_2 <= -6){a = PA_TABLE_868_2[4];}
else if (pa_2 > -6 && pa_2 <= 0){a = PA_TABLE_868_2[5];}
else if (pa_2 > 0 && pa_2 <= 5){a = PA_TABLE_868_2[6];}
else if (pa_2 > 5 && pa_2 <= 7){a = PA_TABLE_868_2[7];}
else if (pa_2 > 7 && pa_2 <= 10){a = PA_TABLE_868_2[8];}
else if (pa_2 > 10){a = PA_TABLE_868_2[9];}
last_pa_2 = 3;
}
else if (MHz_2 >= 900 && MHz_2 <= 928){
if (pa_2 <= -30){a = PA_TABLE_915_2[0];}
else if (pa_2 > -30 && pa_2 <= -20){a = PA_TABLE_915_2[1];}
else if (pa_2 > -20 && pa_2 <= -15){a = PA_TABLE_915_2[2];}
else if (pa_2 > -15 && pa_2 <= -10){a = PA_TABLE_915_2[3];}
else if (pa_2 > -10 && pa_2 <= -6){a = PA_TABLE_915_2[4];}
else if (pa_2 > -6 && pa_2 <= 0){a = PA_TABLE_915_2[5];}
else if (pa_2 > 0 && pa_2 <= 5){a = PA_TABLE_915_2[6];}
else if (pa_2 > 5 && pa_2 <= 7){a = PA_TABLE_915_2[7];}
else if (pa_2 > 7 && pa_2 <= 10){a = PA_TABLE_915_2[8];}
else if (pa_2 > 10){a = PA_TABLE_915_2[9];}
last_pa_2 = 4;
}
if (modulation_2 == 2){
PA_TABLE_2[0] = 0;  
PA_TABLE_2[1] = a;
}else{
PA_TABLE_2[0] = a;  
PA_TABLE_2[1] = 0; 
}
SpiWriteBurstReg(CC1101_PATABLE,PA_TABLE_2,8);
}
/****************************************************************
*FUNCTION NAME:Frequency Calculator
*FUNCTION     :Calculate the basic frequency.
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setMHZ(float mhz){
byte freq2 = 0;
byte freq1 = 0;
byte freq0 = 0;

MHz_2 = mhz;

for (bool i = 0; i==0;){
if (mhz >= 26){
mhz-=26;
freq2+=1;
}
else if (mhz >= 0.1015625){
mhz-=0.1015625;
freq1+=1;
}
else if (mhz >= 0.00039675){
mhz-=0.00039675;
freq0+=1;
}
else{i=1;}
}
if (freq0 > 255){freq1+=1;freq0-=256;}

SpiWriteReg(CC1101_FREQ2, freq2);
SpiWriteReg(CC1101_FREQ1, freq1);
SpiWriteReg(CC1101_FREQ0, freq0);

Calibrate();
}
/****************************************************************
*FUNCTION NAME:Calibrate
*FUNCTION     :Calibrate frequency
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::Calibrate(void){

if (MHz_2 >= 300 && MHz_2 <= 348){
SpiWriteReg(CC1101_FSCTRL0, map(MHz_2, 300, 348, clb1_2[0], clb1_2[1]));
if (MHz_2 < 322.88){SpiWriteReg(CC1101_TEST0,0x0B);}
else{
SpiWriteReg(CC1101_TEST0,0x09);
int s = ELECHOUSE_cc1101_2.SpiReadStatus(CC1101_FSCAL2);
if (s<32){SpiWriteReg(CC1101_FSCAL2, s+32);}
if (last_pa_2 != 1){setPA(pa_2);}
}
}
else if (MHz_2 >= 378 && MHz_2 <= 464){
SpiWriteReg(CC1101_FSCTRL0, map(MHz_2, 378, 464, clb2_2[0], clb2_2[1]));
if (MHz_2 < 430.5){SpiWriteReg(CC1101_TEST0,0x0B);}
else{
SpiWriteReg(CC1101_TEST0,0x09);
int s = ELECHOUSE_cc1101_2.SpiReadStatus(CC1101_FSCAL2);
if (s<32){SpiWriteReg(CC1101_FSCAL2, s+32);}
if (last_pa_2 != 2){setPA(pa_2);}
}
}
else if (MHz_2 >= 779 && MHz_2 <= 899.99){
SpiWriteReg(CC1101_FSCTRL0, map(MHz_2, 779, 899, clb3_2[0], clb3_2[1]));
if (MHz_2 < 861){SpiWriteReg(CC1101_TEST0,0x0B);}
else{
SpiWriteReg(CC1101_TEST0,0x09);
int s = ELECHOUSE_cc1101_2.SpiReadStatus(CC1101_FSCAL2);
if (s<32){SpiWriteReg(CC1101_FSCAL2, s+32);}
if (last_pa_2 != 3){setPA(pa_2);}
}
}
else if (MHz_2 >= 900 && MHz_2 <= 928){
SpiWriteReg(CC1101_FSCTRL0, map(MHz_2, 900, 928, clb4_2[0], clb4_2[1]));
SpiWriteReg(CC1101_TEST0,0x09);
int s = ELECHOUSE_cc1101_2.SpiReadStatus(CC1101_FSCAL2);
if (s<32){SpiWriteReg(CC1101_FSCAL2, s+32);}
if (last_pa_2 != 4){setPA(pa_2);}
}
}
/****************************************************************
*FUNCTION NAME:Calibration offset
*FUNCTION     :Set calibration offset
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setClb(byte b, byte s, byte e){
if (b == 1){
clb1_2[0]=s;
clb1_2[1]=e;  
}
else if (b == 2){
clb2_2[0]=s;
clb2_2[1]=e;  
}
else if (b == 3){
clb3_2[0]=s;
clb3_2[1]=e;  
}
else if (b == 4){
clb4_2[0]=s;
clb4_2[1]=e;  
}
}
/****************************************************************
*FUNCTION NAME:getCC1101
*FUNCTION     :Test Spi connection and return 1 when true.
*INPUT        :none
*OUTPUT       :none
****************************************************************/
bool ELECHOUSE_CC1101_2::getCC1101(void){
setSpi();
if (SpiReadStatus(0x31)>0){
return 1;
}else{
return 0;
}
}
/****************************************************************
*FUNCTION NAME:getMode
*FUNCTION     :Return the Mode. Sidle = 0, TX = 1, Rx = 2.
*INPUT        :none
*OUTPUT       :none
****************************************************************/
byte ELECHOUSE_CC1101_2::getMode(void){
return trxstate_2;
}
/****************************************************************
*FUNCTION NAME:Set Sync_Word
*FUNCTION     :Sync Word
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setSyncWord(byte sh, byte sl){
SpiWriteReg(CC1101_SYNC1, sh);
SpiWriteReg(CC1101_SYNC0, sl);
}
/****************************************************************
*FUNCTION NAME:Set ADDR
*FUNCTION     :Address used for packet filtration. Optional broadcast addresses are 0 (0x00) and 255 (0xFF).
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setAddr(byte v){
SpiWriteReg(CC1101_ADDR, v);
}
/****************************************************************
*FUNCTION NAME:Set PQT
*FUNCTION     :Preamble quality estimator threshold
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setPQT(byte v){
Split_PKTCTRL1();
pc1PQT_2 = 0;
if (v>7){v=7;}
pc1PQT_2 = v*32;
SpiWriteReg(CC1101_PKTCTRL1, pc1PQT_2+pc1CRC_AF_2+pc1APP_ST_2+pc1ADRCHK_2);
}
/****************************************************************
*FUNCTION NAME:Set CRC_AUTOFLUSH
*FUNCTION     :Enable automatic flush of RX FIFO when CRC is not OK
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setCRC_AF(bool v){
Split_PKTCTRL1();
pc1CRC_AF_2 = 0;
if (v==1){pc1CRC_AF_2=8;}
SpiWriteReg(CC1101_PKTCTRL1, pc1PQT_2+pc1CRC_AF_2+pc1APP_ST_2+pc1ADRCHK_2);
}
/****************************************************************
*FUNCTION NAME:Set APPEND_STATUS
*FUNCTION     :When enabled, two status bytes will be appended to the payload of the packet
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setAppendStatus(bool v){
Split_PKTCTRL1();
pc1APP_ST_2 = 0;
if (v==1){pc1APP_ST_2=4;}
SpiWriteReg(CC1101_PKTCTRL1, pc1PQT_2+pc1CRC_AF_2+pc1APP_ST_2+pc1ADRCHK_2);
}
/****************************************************************
*FUNCTION NAME:Set ADR_CHK
*FUNCTION     :Controls address check configuration of received packages
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setAdrChk(byte v){
Split_PKTCTRL1();
pc1ADRCHK_2 = 0;
if (v>3){v=3;}
pc1ADRCHK_2 = v;
SpiWriteReg(CC1101_PKTCTRL1, pc1PQT_2+pc1CRC_AF_2+pc1APP_ST_2+pc1ADRCHK_2);
}
/****************************************************************
*FUNCTION NAME:Set WHITE_DATA
*FUNCTION     :Turn data whitening on / off.
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setWhiteData(bool v){
Split_PKTCTRL0();
pc0WDATA_2 = 0;
if (v == 1){pc0WDATA_2=64;}
SpiWriteReg(CC1101_PKTCTRL0, pc0WDATA_2+pc0PktForm_2+pc0CRC_EN_2+pc0LenConf_2);
}
/****************************************************************
*FUNCTION NAME:Set PKT_FORMAT
*FUNCTION     :Format of RX and TX data
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setPktFormat(byte v){
Split_PKTCTRL0();
pc0PktForm_2 = 0;
if (v>3){v=3;}
pc0PktForm_2 = v*16;
SpiWriteReg(CC1101_PKTCTRL0, pc0WDATA_2+pc0PktForm_2+pc0CRC_EN_2+pc0LenConf_2);
}
/****************************************************************
*FUNCTION NAME:Set CRC
*FUNCTION     :CRC calculation in TX and CRC check in RX
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setCrc(bool v){
Split_PKTCTRL0();
pc0CRC_EN_2 = 0;
if (v==1){pc0CRC_EN_2=4;}
SpiWriteReg(CC1101_PKTCTRL0, pc0WDATA_2+pc0PktForm_2+pc0CRC_EN_2+pc0LenConf_2);
}
/****************************************************************
*FUNCTION NAME:Set LENGTH_CONFIG
*FUNCTION     :Configure the packet length
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setLengthConfig(byte v){
Split_PKTCTRL0();
pc0LenConf_2 = 0;
if (v>3){v=3;}
pc0LenConf_2 = v;
SpiWriteReg(CC1101_PKTCTRL0, pc0WDATA_2+pc0PktForm_2+pc0CRC_EN_2+pc0LenConf_2);
}
/****************************************************************
*FUNCTION NAME:Set PACKET_LENGTH
*FUNCTION     :Indicates the packet length
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setPacketLength(byte v){
SpiWriteReg(CC1101_PKTLEN, v);
}
/****************************************************************
*FUNCTION NAME:Set DCFILT_OFF
*FUNCTION     :Disable digital DC blocking filter before demodulator
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setDcFilterOff(bool v){
Split_MDMCFG2();
m2DCOFF_2 = 0;
if (v==1){m2DCOFF_2=128;}
SpiWriteReg(CC1101_MDMCFG2, m2DCOFF_2+m2MODFM_2+m2MANCH_2+m2SYNCM_2);
}
/****************************************************************
*FUNCTION NAME:Set MANCHESTER
*FUNCTION     :Enables Manchester encoding/decoding
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setManchester(bool v){
Split_MDMCFG2();
m2MANCH_2 = 0;
if (v==1){m2MANCH_2=8;}
SpiWriteReg(CC1101_MDMCFG2, m2DCOFF_2+m2MODFM_2+m2MANCH_2+m2SYNCM_2);
}
/****************************************************************
*FUNCTION NAME:Set SYNC_MODE
*FUNCTION     :Combined sync-word qualifier mode
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setSyncMode(byte v){
Split_MDMCFG2();
m2SYNCM_2 = 0;
if (v>7){v=7;}
m2SYNCM_2=v;
SpiWriteReg(CC1101_MDMCFG2, m2DCOFF_2+m2MODFM_2+m2MANCH_2+m2SYNCM_2);
}
/****************************************************************
*FUNCTION NAME:Set FEC
*FUNCTION     :Enable Forward Error Correction (FEC)
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setFEC(bool v){
Split_MDMCFG1();
m1FEC_2=0;
if (v==1){m1FEC_2=128;}
SpiWriteReg(CC1101_MDMCFG1, m1FEC_2+m1PRE_2+m1CHSP_2);
}
/****************************************************************
*FUNCTION NAME:Set PRE
*FUNCTION     :Sets the minimum number of preamble bytes to be transmitted.
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setPRE(byte v){
Split_MDMCFG1();
m1PRE_2=0;
if (v>7){v=7;}
m1PRE_2 = v*16;
SpiWriteReg(CC1101_MDMCFG1, m1FEC_2+m1PRE_2+m1CHSP_2);
}
/****************************************************************
*FUNCTION NAME:Set Channel
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setChannel(byte ch){
chan_2 = ch;
SpiWriteReg(CC1101_CHANNR,   chan_2);
}
/****************************************************************
*FUNCTION NAME:Set Channel spacing
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setChsp(float f){
Split_MDMCFG1();
byte MDMCFG0 = 0;
m1CHSP_2 = 0;
if (f > 405.456543){f = 405.456543;}
if (f < 25.390625){f = 25.390625;}
for (int i = 0; i<5; i++){
if (f <= 50.682068){
f -= 25.390625;
f /= 0.0991825;
MDMCFG0 = f;
float s1 = (f - MDMCFG0) *10;
if (s1 >= 5){MDMCFG0++;}
i = 5;
}else{
m1CHSP_2++;
f/=2;
}
}
SpiWriteReg(19,m1CHSP_2+m1FEC_2+m1PRE_2);
SpiWriteReg(20,MDMCFG0);
}
/****************************************************************
*FUNCTION NAME:Set Receive bandwidth
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setRxBW(float f){
Split_MDMCFG4();
int s1 = 3;
int s2 = 3;
for (int i = 0; i<3; i++){
if (f > 101.5625){f/=2; s1--;}
else{i=3;}
}
for (int i = 0; i<3; i++){
if (f > 58.1){f/=1.25; s2--;}
else{i=3;}
}
s1 *= 64;
s2 *= 16;
m4RxBw_2 = s1 + s2;
SpiWriteReg(16,m4RxBw_2+m4DaRa_2);
}
/****************************************************************
*FUNCTION NAME:Set Data Rate
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setDRate(float d){
Split_MDMCFG4();
float c = d;
byte MDMCFG3 = 0;
if (c > 1621.83){c = 1621.83;}
if (c < 0.0247955){c = 0.0247955;}
m4DaRa_2 = 0;
for (int i = 0; i<20; i++){
if (c <= 0.0494942){
c = c - 0.0247955;
c = c / 0.00009685;
MDMCFG3 = c;
float s1 = (c - MDMCFG3) *10;
if (s1 >= 5){MDMCFG3++;}
i = 20;
}else{
m4DaRa_2++;
c = c/2;
}
}
SpiWriteReg(16,  m4RxBw_2+m4DaRa_2);
SpiWriteReg(17,  MDMCFG3);
}
/****************************************************************
*FUNCTION NAME:Set Devitation
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setDeviation(float d){
float f = 1.586914;
float v = 0.19836425;
int c = 0;
if (d > 380.859375){d = 380.859375;}
if (d < 1.586914){d = 1.586914;}
for (int i = 0; i<255; i++){
f+=v;
if (c==7){v*=2;c=-1;i+=8;}
if (f>=d){c=i;i=255;}
c++;
}
SpiWriteReg(21,c);
}
/****************************************************************
*FUNCTION NAME:Split PKTCTRL0
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::Split_PKTCTRL1(void){
int calc = SpiReadStatus(7);
pc1PQT_2 = 0;
pc1CRC_AF_2 = 0;
pc1APP_ST_2 = 0;
pc1ADRCHK_2 = 0;
for (bool i = 0; i==0;){
if (calc >= 32){calc-=32; pc1PQT_2+=32;}
else if (calc >= 8){calc-=8; pc1CRC_AF_2+=8;}
else if (calc >= 4){calc-=4; pc1APP_ST_2+=4;}
else {pc1ADRCHK_2 = calc; i=1;}
}
}
/****************************************************************
*FUNCTION NAME:Split PKTCTRL0
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::Split_PKTCTRL0(void){
int calc = SpiReadStatus(8);
pc0WDATA_2 = 0;
pc0PktForm_2 = 0;
pc0CRC_EN_2 = 0;
pc0LenConf_2 = 0;
for (bool i = 0; i==0;){
if (calc >= 64){calc-=64; pc0WDATA_2+=64;}
else if (calc >= 16){calc-=16; pc0PktForm_2+=16;}
else if (calc >= 4){calc-=4; pc0CRC_EN_2+=4;}
else {pc0LenConf_2 = calc; i=1;}
}
}
/****************************************************************
*FUNCTION NAME:Split MDMCFG1
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::Split_MDMCFG1(void){
int calc = SpiReadStatus(19);
m1FEC_2 = 0;
m1PRE_2 = 0;
m1CHSP_2 = 0;
int s2 = 0;
for (bool i = 0; i==0;){
if (calc >= 128){calc-=128; m1FEC_2+=128;}
else if (calc >= 16){calc-=16; m1PRE_2+=16;}
else {m1CHSP_2 = calc; i=1;}
}
}
/****************************************************************
*FUNCTION NAME:Split MDMCFG2
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::Split_MDMCFG2(void){
int calc = SpiReadStatus(18);
m2DCOFF_2 = 0;
m2MODFM_2 = 0;
m2MANCH_2 = 0;
m2SYNCM_2 = 0;
for (bool i = 0; i==0;){
if (calc >= 128){calc-=128; m2DCOFF_2+=128;}
else if (calc >= 16){calc-=16; m2MODFM_2+=16;}
else if (calc >= 8){calc-=8; m2MANCH_2+=8;}
else{m2SYNCM_2 = calc; i=1;}
}
}
/****************************************************************
*FUNCTION NAME:Split MDMCFG4
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::Split_MDMCFG4(void){
int calc = SpiReadStatus(16);
m4RxBw_2 = 0;
m4DaRa_2 = 0;
for (bool i = 0; i==0;){
if (calc >= 64){calc-=64; m4RxBw_2+=64;}
else if (calc >= 16){calc -= 16; m4RxBw_2+=16;}
else{m4DaRa_2 = calc; i=1;}
}
}
/****************************************************************
*FUNCTION NAME:RegConfigSettings
*FUNCTION     :CC1101 register config //details refer datasheet of CC1101/CC1100//
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::RegConfigSettings(void) 
{   
    SpiWriteReg(CC1101_FSCTRL1,  0x06);
    
    setCCMode(ccmode_2);
    setMHZ(MHz_2);
    
    SpiWriteReg(CC1101_MDMCFG1,  0x02);
    SpiWriteReg(CC1101_MDMCFG0,  0xF8);
    SpiWriteReg(CC1101_CHANNR,   chan_2);
    SpiWriteReg(CC1101_DEVIATN,  0x47);
    SpiWriteReg(CC1101_FREND1,   0x56);
    SpiWriteReg(CC1101_MCSM0 ,   0x18);
    SpiWriteReg(CC1101_FOCCFG,   0x16);
    SpiWriteReg(CC1101_BSCFG,    0x1C);
    SpiWriteReg(CC1101_AGCCTRL2, 0xC7);
    SpiWriteReg(CC1101_AGCCTRL1, 0x00);
    SpiWriteReg(CC1101_AGCCTRL0, 0xB2);
    SpiWriteReg(CC1101_FSCAL3,   0xE9);
    SpiWriteReg(CC1101_FSCAL2,   0x2A);
    SpiWriteReg(CC1101_FSCAL1,   0x00);
    SpiWriteReg(CC1101_FSCAL0,   0x1F);
    SpiWriteReg(CC1101_FSTEST,   0x59);
    SpiWriteReg(CC1101_TEST2,    0x81);
    SpiWriteReg(CC1101_TEST1,    0x35);
    SpiWriteReg(CC1101_TEST0,    0x09);
    SpiWriteReg(CC1101_PKTCTRL1, 0x04);
    SpiWriteReg(CC1101_ADDR,     0x00);
    SpiWriteReg(CC1101_PKTLEN,   0x00);
}
/****************************************************************
*FUNCTION NAME:SetTx
*FUNCTION     :set CC1101 send data
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SetTx(void)
{
  SpiStrobe(CC1101_SIDLE);
  SpiStrobe(CC1101_STX);        //start send
  trxstate_2=1;
}
/****************************************************************
*FUNCTION NAME:SetRx
*FUNCTION     :set CC1101 to receive state
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SetRx(void)
{
  SpiStrobe(CC1101_SIDLE);
  SpiStrobe(CC1101_SRX);        //start receive
  trxstate_2=2;
}
/****************************************************************
*FUNCTION NAME:SetTx
*FUNCTION     :set CC1101 send data and change frequency
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SetTx(float mhz)
{
  SpiStrobe(CC1101_SIDLE);
  setMHZ(mhz);
  SpiStrobe(CC1101_STX);        //start send
  trxstate_2=1;
}
/****************************************************************
*FUNCTION NAME:SetRx
*FUNCTION     :set CC1101 to receive state and change frequency
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SetRx(float mhz)
{
  SpiStrobe(CC1101_SIDLE);
  setMHZ(mhz);
  SpiStrobe(CC1101_SRX);        //start receive
  trxstate_2=2;
}
/****************************************************************
*FUNCTION NAME:RSSI Level
*FUNCTION     :Calculating the RSSI Level
*INPUT        :none
*OUTPUT       :none
****************************************************************/
int ELECHOUSE_CC1101_2::getRssi(void)
{
int rssi;
rssi=SpiReadStatus(CC1101_RSSI);
if (rssi >= 128){rssi = (rssi-256)/2-74;}
else{rssi = (rssi/2)-74;}
return rssi;
}
/****************************************************************
*FUNCTION NAME:LQI Level
*FUNCTION     :get Lqi state
*INPUT        :none
*OUTPUT       :none
****************************************************************/
byte ELECHOUSE_CC1101_2::getLqi(void)
{
byte lqi;
lqi=SpiReadStatus(CC1101_LQI);
return lqi;
}
/****************************************************************
*FUNCTION NAME:SetSres
*FUNCTION     :Reset CC1101
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setSres(void)
{
  SpiStrobe(CC1101_SRES);
  trxstate_2=0;
}
/****************************************************************
*FUNCTION NAME:setSidle
*FUNCTION     :set Rx / TX Off
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::setSidle(void)
{
  SpiStrobe(CC1101_SIDLE);
  trxstate_2=0;
}
/****************************************************************
*FUNCTION NAME:goSleep
*FUNCTION     :set cc1101 Sleep on
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::goSleep(void){
  trxstate_2=0;
  SpiStrobe(0x36);//Exit RX / TX, turn off frequency synthesizer and exit
  SpiStrobe(0x39);//Enter power down mode when CSn goes high.
}
/****************************************************************
*FUNCTION NAME:Char direct SendData
*FUNCTION     :use CC1101 send data
*INPUT        :txBuffer: data array to send; size: number of data to send, no more than 61
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SendData(char *txchar)
{
int len = strlen(txchar);
byte chartobyte[len];
for (int i = 0; i<len; i++){chartobyte[i] = txchar[i];}
SendData(chartobyte,len);
}
/****************************************************************
*FUNCTION NAME:SendData
*FUNCTION     :use CC1101 send data
*INPUT        :txBuffer: data array to send; size: number of data to send, no more than 61
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SendData(byte *txBuffer,byte size)
{
  SpiWriteReg(CC1101_TXFIFO,size);
  SpiWriteBurstReg(CC1101_TXFIFO,txBuffer,size);      //write data to send
  SpiStrobe(CC1101_SIDLE);
  SpiStrobe(CC1101_STX);                  //start send
    while (!digitalRead(GDO0_2));               // Wait for GDO0_2 to be set -> sync transmitted  
    while (digitalRead(GDO0_2));                // Wait for GDO0_2 to be cleared -> end of packet
  SpiStrobe(CC1101_SFTX);                 //flush TXfifo
  trxstate_2=1;
}
/****************************************************************
*FUNCTION NAME:Char direct SendData
*FUNCTION     :use CC1101 send data without GDO
*INPUT        :txBuffer: data array to send; size: number of data to send, no more than 61
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SendData(char *txchar,int t)
{
int len = strlen(txchar);
byte chartobyte[len];
for (int i = 0; i<len; i++){chartobyte[i] = txchar[i];}
SendData(chartobyte,len,t);
}
/****************************************************************
*FUNCTION NAME:SendData
*FUNCTION     :use CC1101 send data without GDO
*INPUT        :txBuffer: data array to send; size: number of data to send, no more than 61
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101_2::SendData(byte *txBuffer,byte size,int t)
{
  SpiWriteReg(CC1101_TXFIFO,size);
  SpiWriteBurstReg(CC1101_TXFIFO,txBuffer,size);      //write data to send
  SpiStrobe(CC1101_SIDLE);
  SpiStrobe(CC1101_STX);                  //start send
  delay(t);
  SpiStrobe(CC1101_SFTX);                 //flush TXfifo
  trxstate_2=1;
}
/****************************************************************
*FUNCTION NAME:Check CRC
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
bool ELECHOUSE_CC1101_2::CheckCRC(void){
byte lqi=SpiReadStatus(CC1101_LQI);
bool crc_ok = bitRead(lqi,7);
if (crc_ok == 1){
return 1;
}else{
SpiStrobe(CC1101_SFRX);
SpiStrobe(CC1101_SRX);
return 0;
}
}
/****************************************************************
*FUNCTION NAME:CheckRxFifo
*FUNCTION     :check receive data or not
*INPUT        :none
*OUTPUT       :flag: 0 no data; 1 receive data 
****************************************************************/
bool ELECHOUSE_CC1101_2::CheckRxFifo(int t){
if(trxstate_2!=2){SetRx();}
if(SpiReadStatus(CC1101_RXBYTES) & BYTES_IN_RXFIFO){
delay(t);
return 1;
}else{
return 0;
}
}
/****************************************************************
*FUNCTION NAME:CheckReceiveFlag
*FUNCTION     :check receive data or not
*INPUT        :none
*OUTPUT       :flag: 0 no data; 1 receive data 
****************************************************************/
byte ELECHOUSE_CC1101_2::CheckReceiveFlag(void)
{
  if(trxstate_2!=2){SetRx();}
	if(digitalRead(GDO0_2))			//receive data
	{
		while (digitalRead(GDO0_2));
		return 1;
	}
	else							// no data
	{
		return 0;
	}
}
/****************************************************************
*FUNCTION NAME:ReceiveData
*FUNCTION     :read data received from RXfifo
*INPUT        :rxBuffer: buffer to store data
*OUTPUT       :size of data received
****************************************************************/
byte ELECHOUSE_CC1101_2::ReceiveData(byte *rxBuffer)
{
	byte size;
	byte status[2];

	if(SpiReadStatus(CC1101_RXBYTES) & BYTES_IN_RXFIFO)
	{
		size=SpiReadReg(CC1101_RXFIFO);
		SpiReadBurstReg(CC1101_RXFIFO,rxBuffer,size);
		SpiReadBurstReg(CC1101_RXFIFO,status,2);
		SpiStrobe(CC1101_SFRX);
    SpiStrobe(CC1101_SRX);
		return size;
	}
	else
	{
		SpiStrobe(CC1101_SFRX);
    SpiStrobe(CC1101_SRX);
 		return 0;
	}
}
ELECHOUSE_CC1101_2 ELECHOUSE_cc1101_2;
