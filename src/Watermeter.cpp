/*
  Watermeter.cpp - Control Library for reed switch pulse actuated water meters 
  This one designed around the DAE Water Meter Model V-100P
  Most of these meters operate in the same manner, the difference
  can be adjusted as parameters in the instance creation.
  https://www.youtube.com/c/jordanrubin6502
  2020 Jordan Rubin.
  */
#define FORMAT_SPIFFS_IF_FAILED true

#include <Arduino.h> // Required for Platform.io
#include <Watermeter.h>
#include <SPIFFS.h>

int meterSignal;
bool gallon;
bool update;
long lastDebounce = 0;    // Holds last debounce
long debounceDelay;  // Between re-polling
double meter = 0;
float increment;
bool useSPIFFS;
void IRAM_ATTR respondInterrupt();

//CONSTRUCTOR -----------------------------------------------------------
//WATERMETER WATERMETER(SignalGPIOpin,useInternalPullupResistor,measure[g|l],debouncedelay,useSPIFFS,incriment)
WATERMETER::WATERMETER(int signalGPIOpin, bool useInternalPullups,char measure, long dbounce ,bool useSPIF, float incr)
{
  //----------------- initialize initial parameters  
    if (useInternalPullups){
      pinMode(signalGPIOpin, INPUT_PULLUP);
    }
    else {
      pinMode(signalGPIOpin, INPUT);
    }
    if (measure == 'g'){gallon = 1;}
    else {gallon = 0;}
    attachInterrupt(signalGPIOpin, respondInterrupt, FALLING);
    meterSignal = signalGPIOpin;
    increment = incr;
    debounceDelay = dbounce;
    useSPIFFS = useSPIF;
}
// ----------------------------------------------------------------------------]

// FUNCTION - [updated] - [Checks and responds to the update flag--------------]
bool WATERMETER::updated(void){
  if (update){
    writeFile();
    update = 0;
    return 1;    
  }
  else {
    return 0;
  }
} 
// ----------------------------------------------------------------------------]

// FUNCTION - [getDebounce] - [returns the Debounce delay in mS----------------]
int WATERMETER::getDebounce(void){
  return debounceDelay;
} 
// ----------------------------------------------------------------------------]

// FUNCTION - [initFilesys] - [Starts up the Meter File system and test--------]
const char* WATERMETER::initFilesys(void){
     if (useSPIFFS){
      if (!SPIFFS.begin(true)){
        return "ERR";
      }
//DELETE FOR TESTING
//SPIFFS.remove("/meter.val");      
      if(!SPIFFS.exists("/meter.val")){    
        writeFile();
        return "NEW";
      }
      else {
        bool reset = readFile();
        if (reset){return "RESET";}
      }
  }
  else {
    return "NONE";
  }
  return "OK";
}
// ----------------------------------------------------------------------------]

// FUNCTION - [readFile] - [Opens and reads meter file into meter value--------]
bool WATERMETER::readFile(){
  bool resetalarm=0;
  File meterfile = SPIFFS.open("/meter.val","r");
  char test[20];
  int i=0;
  while (meterfile.available()) {
    char B = meterfile.read();
    test[i] = B;
    i++;
    if (i >=15) {
      resetalarm=1;
      SPIFFS.remove("/meter.val");
      writeFile();
      return resetalarm;
    }
  }      
  test[i] = '\0';
  meterfile.close();
  char* end; 
  double number; 
  number = strtod(test, &end);
  meter = number;
  return resetalarm;
}
// ----------------------------------------------------------------------------]

// FUNCTION - [readOut] - [Returns the current value on the meter--------------]
double WATERMETER::readOut(void){
  return meter;
}
// ----------------------------------------------------------------------------]

// FUNCTION - [readOut] - [Returns the current value on the meter converted----]
double WATERMETER::readOut(char type){
  if (gallon){
    if (type == 'g'){return meter;}
    return (meter*3.78541);
  }
  else {
    if (type == 'l'){return meter;}
    return (meter*0.264172);
  }
}
// ----------------------------------------------------------------------------]

// HANDLER -  [respondInterrupt] - [-Code when reed switch is triggered--------]
void IRAM_ATTR respondInterrupt(){
  if ((millis() - lastDebounce) > debounceDelay) {  
    meter = meter+increment;
    update = 1;
    lastDebounce = millis();
  }
}
// ----------------------------------------------------------------------------]

// FUNCTION - [setDebounce] - [Sets the Debounce delay in mS-------------------]
void WATERMETER::setDebounce(int value){
  debounceDelay = value;
} 
// ----------------------------------------------------------------------------]

// FUNCTION - [setMeter] - [Sets the meter from the physical display-----------]
void WATERMETER::setMeter(double value){
  meter = value;
} 
// ----------------------------------------------------------------------------]

// FUNCTION - [writeFile] - [Opens or creates meter file, writes value---------]
void WATERMETER::writeFile(){
  File  meterfile = SPIFFS.open("/meter.val","w");
  char convert[10];
  sprintf(convert, "%6.2f", meter);
  meterfile.print(convert);
  meterfile.close();
}
// ----------------------------------------------------------------------------]
