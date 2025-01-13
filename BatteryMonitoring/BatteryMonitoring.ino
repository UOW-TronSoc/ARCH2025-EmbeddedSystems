
/* The Core Monitoring code*/
#include <Wire.h>
#include <INA226_WE.h>   
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "string.h"
#include "batterymonitoring.h"
#define I2C_ADDRESS 0x40

#define REASSIGN_PINS
int sck = 14;
int miso = 12;
int mosi = 13;
int cs = 5;

INA226_WE ina226 = INA226_WE(I2C_ADDRESS);
  float Ah = 1.8; // set to what ever the battery is 
  float maxCharge = 3600 * Ah;
  float shuntVoltage_mV = 0.0;
  float loadVoltage_V = 0.0;
  float busVoltage_V = 0.0;
  float current_mA = 0.0;
  float power_mW = 0.0;
  float ti=0;
  float duration=0;
  double lastti=0;
  double cpi=0; // coulombs per tick
  double soc=0; // state of charge
  float timeLeft, prevTimeLeft;
  float avgTimeLeft = 0;
  int alpha = 0.1;

void setup() 
{
  Serial.begin(115200); // for the esp32
  while (!Serial) {
    delay(10);
  } // throw a tantrum is serial isnt going

#ifdef REASSIGN_PINS
  SPI.begin(sck, miso, mosi, cs);
  if (!SD.begin(cs)) {
#else
  if (!SD.begin()) {
#endif
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  setupFile(SD,"/batt_states");


  //Serial.begin(9600);
  Wire.begin();
  ina226.init();

  ina226.setAverage(AVERAGE_64); // this is the number of measurements that will be averaged. By default its only 1 

  ina226.setMeasureMode(CONTINUOUS);  
  /* Set Resistor and Current Range
    if resistor is 5.0 mOhm, current range is up to 10.0 A
    default is 100 mOhm and about 1.3 A*/
  ina226.setResistorRange(0.004, 20); // choose resistor 0.004 Ohm and gain range up to 20A

  /* If the current values delivered by the INA226 differ by a constant factor
    from values obtained with calibrated equipment you can define a correction factor.
    Correction factor = current delivered from calibrated equipment / current delivered by INA226*/
  ina226.setCorrectionFactor(0.93);

  ina226.waitUntilConversionCompleted(); //if you comment this line the first data might be zero, so leave it
}

void loop()
{
  ina226.readAndClearFlags();
  shuntVoltage_mV = ina226.getShuntVoltage_mV(); // read voltage from the 226
  busVoltage_V = ina226.getBusVoltage_V(); // maybe not needed, but reads the voltage from the battery basically
  current_mA = ina226.getCurrent_mA();

  power_mW = ina226.getBusPower(); // so this does the multiplication for you, wow, saved some esp32 processing time
  loadVoltage_V  = busVoltage_V + (shuntVoltage_mV / 1000);

  //delay(3000);


  ti=millis();
  duration = ti - lastti;
  lastti = ti;

  if(current_mA <0){current_mA =0;};

  cpi += (current_mA/1000) * (duration/1000) ;// coulombs 
  soc = maxCharge-cpi;

  timeLeft = soc/(current_mA/1000);
  avgTimeLeft = timeLeft *alpha + ((1-alpha) *prevTimeLeft);
  prevTimeLeft = timeLeft;

  Serial.print("SOC:  %");
  Serial.print(soc/maxCharge *100); 

  Serial.print("\t CPI:  ");
  Serial.print(cpi); 

  Serial.print("\t mA:  ");
  Serial.print(current_mA); 

  Serial.print("\t Time Left: ");
  Serial.print(timeLeft/60);
  Serial.print("\t avg Time Left: ");
  Serial.print(avgTimeLeft/60);

  Serial.print("\t Bat V: ");
  Serial.println(busVoltage_V);





  delay(1000);
}


/* This is the display code




void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

#ifdef REASSIGN_PINS
  SPI.begin(sck, miso, mosi, cs);
  if (!SD.begin(cs)) {
#else
  if (!SD.begin()) {
#endif
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  setupFile(SD,"/batt_states");
}

void loop() {
  // read battery states from SD, put into char arrays
  readFile(SD,"/batt_states");
  printchar("after sd reading: ");

  // copy char arrays into doubles
  Batts_double.batt1 = atof(Batts_char.batt1);
  Batts_double.batt2 = atof(Batts_char.batt2);
  Batts_double.batt3 = atof(Batts_char.batt3);
  Batts_double.batt4 = atof(Batts_char.batt4);
  Batts_double.batt5 = atof(Batts_char.batt5);
  printdub("after double conversion: ");

  // subtract ammeter values from battery states
  Batts_double.batt1 = Batts_double.batt1 + Ammeter_readings.amm1;
  Batts_double.batt2 = Batts_double.batt2 + Ammeter_readings.amm2;
  Batts_double.batt3 = Batts_double.batt3 + Ammeter_readings.amm3;
  Batts_double.batt4 = Batts_double.batt4 + Ammeter_readings.amm4;
  Batts_double.batt5 = Batts_double.batt5 + Ammeter_readings.amm5;

  if(Batts_double.batt1 <= 0){
    Batts_double.batt1 = 100;
  }
  if(Batts_double.batt2 <= 0){
    Batts_double.batt2 = 100;
  }
  if(Batts_double.batt3 <= 0){
    Batts_double.batt3 = 100;
  }
  if(Batts_double.batt4 <= 0){
    Batts_double.batt4 = 100;
  }
  if(Batts_double.batt5 <= 0){
    Batts_double.batt5 = 100;
  }
  printdub("after math: ");

  //fill char arrays with new values
  dtostrf(Batts_double.batt1,6,2,Batts_char.batt1);
  dtostrf(Batts_double.batt2,6,2,Batts_char.batt2);
  dtostrf(Batts_double.batt3,6,2,Batts_char.batt3);
  dtostrf(Batts_double.batt4,6,2,Batts_char.batt4);
  dtostrf(Batts_double.batt5,6,2,Batts_char.batt5);
  printchar("after converting to char: ");

  //save values to the SD card
  writeFile(SD,"/batt_states");

  delay(5000);
}*/