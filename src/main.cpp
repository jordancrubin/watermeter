/*
  testmeter.ino/cpp - Test programme for reed switch pulse actuated water meters 
  This one designed around the DAE Water Meter Model V-100P
  Most of these meters operate in the same manner, the difference
  can be adjusted as parameters in the instance creation.
  https://www.youtube.com/c/jordanrubin6502
  2020 Jordan Rubin.
  */
#include <Arduino.h> //required for platformio
#include <Watermeter.h>

//WATERMETER WATERMETER(SignalGPIOpin,useInternalPullupResistor,measure[g|l],metervalue)
// Using GPIO 15                            [15]
// Using internal pullup resistors          [true]
// This meter is designed as gallons        [g]
// Debounce delay in ms                     [100] 
// Use SPIFFS to autosave meter to filesys  [true]          
// The value of measurment added each time  [.1] i.e. .1 gallons per pulse
// Verbose output to the Serial console     [true] 
WATERMETER thisMeter(15,true,'g',100,true,.1,true);

void setup() {
  Serial.begin(38400);
  Serial.println("\n\n***Water Meter Test Programme***");
  delay(3000);
  thisMeter.setMeter(32.2);
  Serial.println(thisMeter.initFilesys());
}

void loop() {
  Serial.println(thisMeter.readOut());
  thisMeter.checkUpdate();
  delay(1500);
  Serial.println(thisMeter.readOut('l'));
  thisMeter.checkUpdate();
  delay(1500);
}