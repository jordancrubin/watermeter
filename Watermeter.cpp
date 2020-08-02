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
bool verbose = 0;
long lastDebounce   = 0;    // Holds last debounce
long debounceDelay;  // Between re-polling
double meter = 0;
float increment;
bool useSPIFFS;
void IRAM_ATTR respondInterrupt();

//CONSTRUCTOR -----------------------------------------------------------
//WATERMETER WATERMETER(SignalGPIOpin,useInternalPullupResistor,measure[g|l],debouncedelay,useSPIFFS,incriment)
WATERMETER::WATERMETER(int signalGPIOpin, bool useInternalPullups,char measure, long dbounce ,bool useSPIF, float incr, bool verboseval)
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
    verbose = verboseval;
}
// ----------------------------------------------------------------------------]

// FUNCTION - [checkUpdate] - [Checks and responds to the update flag----------]
bool WATERMETER::checkUpdate(void){
  if (update){
    if(verbose){Serial.println("Updating...");}
    writeFile();
    update =0;
    return 1;    
  }
  else {
    if(verbose){Serial.println("No update...");}
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
      if(verbose){Serial.println("Initializing FS...");}
      if (!SPIFFS.begin(true)){
        return "SPIFFS_MOUNT_ERR";
      }
//DELETE FOR TESTING
//SPIFFS.remove("/meter.val");      
      if(!SPIFFS.exists("/meter.val")){   
        if(verbose){Serial.println("Meter file not found. Creating...");}    
        writeFile();
      }
      else {
        if(verbose){Serial.println("Meter file found. Reading value into memory...");}
        readFile();
      }
    return "OK";
  }
  else {
    if(verbose){
        Serial.println("Object constructed without SPIFFS support enabled....");
        return "NO_SPIFFS_SUPPORT";
    }
  }
  return "OK";
}
// ----------------------------------------------------------------------------]

// FUNCTION - [readFile] - [Opens and reads meter file into meter value--------]
void WATERMETER::readFile(){
  File  meterfile = SPIFFS.open("/meter.val","r");
  char test[20];
  int i=0;
  while (meterfile.available()) {
    char B = meterfile.read();
    test[i] = B;
    i++;
    if (i >=15) {
      if(verbose){Serial.println("File corrupted, deleting....");}
      SPIFFS.remove("/meter.val");
      writeFile();
      return;
    }
  }      
  test[i] = '\0';
  meterfile.close();
  char* end; 
  double number; 
  number = strtod(test, &end);
  if(verbose){Serial.print("READING-> "); Serial.println(number);} 
  meter = number;
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
  if(verbose){Serial.print("WRITING-> "); Serial.println(convert);}
  meterfile.print(convert);
  meterfile.close();
}
// ----------------------------------------------------------------------------]
