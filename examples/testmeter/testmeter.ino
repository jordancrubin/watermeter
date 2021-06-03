/*
  Change this to main.cpp to compile test programme in platform.io
  main.cpp - Test programme for reed switch pulse actuated water meters 
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
WATERMETER thisMeter(15,true,'g',100,true,.1);

void setup() {
  Serial.begin(38400);
  Serial.println("\n\n***Water Meter Test Programme***");
  delay(3000);

  //This allows the setting of the meter in the object to a specified value
  //That might match tat of an analogue gauge on the metre
  thisMeter.setMeter(32.2);

/*
  Print the result codes out directly as the init is called
  Serial.println(thisMeter.initFilesys());
*/

  //Or assign the result to a const char and process it after function execution
  const char * result = thisMeter.initFilesys();
  Serial.println(result);

/*
  Or use a methodology to test success directly
  if (strcmp(thisMeter.initFilesys(),"OK")==0){
    Serial.println("SPIFFS STARTED");
}
  The codes are:
  OK -    Everything worked as expected
  ERR-    The SPIFFS system was corrupted and couldnt be mounted
  NEW-    It found no Metre file and created a new one
  RESET-  The file had data errors and was re-initialized
  NONE-   The function was executed but the object has no param for SPIFFS support
*/
}

void loop() {
  //This function is boolean, returns 1 if updated, 0 if not. 
  // Clears update flag when queried.
  if(thisMeter.updated()){
      Serial.println("UPDATE");
      Serial.print("Litres -> ");Serial.println(thisMeter.readOut('l'));
      //Gallons is implied with no param becuase the object was build with g as a param
      Serial.print("Gallons-> ");Serial.println(thisMeter.readOut());
    }
    else {
      Serial.println("NO UPDATE");
    }
  delay(1500);
}