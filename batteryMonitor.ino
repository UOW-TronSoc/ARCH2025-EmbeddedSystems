#include <Wire.h>
#include <INA226_WE.h>   
#define I2C_ADDRESS 0x40
 
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
  Serial.begin(9600);
  while (!Serial); // wait until serial comes up on Arduino Leonardo or MKR WiFi 1010
  Wire.begin();
  ina226.init();
 
 
  /* Set Number of measurements for shunt and bus voltage which shall be averaged
    Mode *     * Number of samples
    AVERAGE_1            1 (default)
    AVERAGE_4            4
    AVERAGE_16          16
    AVERAGE_64          64
    AVERAGE_128        128
    AVERAGE_256        256
    AVERAGE_512        512
    AVERAGE_1024      1024*/
 
  ina226.setAverage(AVERAGE_64); // choose mode and uncomment for change of default
 

 
  //ina226.setMeasureMode(CONTINUOUS); // choose mode and uncomment for change of default
 
  /* Set Resistor and Current Range
     if resistor is 5.0 mOhm, current range is up to 10.0 A
     default is 100 mOhm and about 1.3 A*/
 
  ina226.setResistorRange(0.004, 20); // choose resistor 0.004 Ohm and gain range up to 20A
 
  /* If the current values delivered by the INA226 differ by a constant factor
     from values obtained with calibrated equipment you can define a correction factor.
     Correction factor = current delivered from calibrated equipment / current delivered by INA226*/
 
  ina226.setCorrectionFactor(0.93);
 
  ina226.waitUntilConversionCompleted(); //if you comment this line the first data might be zero
}

void loop()
{
 
  ina226.readAndClearFlags();
  shuntVoltage_mV = ina226.getShuntVoltage_mV();
  busVoltage_V = ina226.getBusVoltage_V();
 // current_mA = ina226.getCurrent_mA();
  current_mA = 100;
  power_mW = ina226.getBusPower();
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
