#include <BlynkSimpleStream.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"
MAX30105 poxSensor;
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

//auth
char auth[] = "Your auth here";
//Widget
WidgetLCD lcd1(V4);
void setup()
{
  Serial.begin(9600);
  poxSensor.begin(Wire, I2C_SPEED_FAST);
  Blynk.begin(Serial, auth);
  poxSensor.setup();
  poxSensor.setPulseAmplitudeRed(0x0A);
  poxSensor.setPulseAmplitudeGreen(0);
}
void loop()
{
  Blynk.run();
  //lcd.print(0, 0, "IR");
  //lcd1.print(0, 0, "BPM");
  long irValue = poxSensor.getIR();
  float temperature = poxSensor.readTemperature();
  Blynk.virtualWrite(V5, temperature);
  //lcd.print(0, 1, irValue);
  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();
    //lcd1.print(0, 0, "BPM");
    beatsPerMinute = 60 / (delta / 1000.0);
    //lcd1.print(0, 1, beatsPerMinute);
    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      lcd1.print(0, 0, "AverageBeat");
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
      lcd1.print(0, 1, beatAvg);
    } 
  }
  
}

