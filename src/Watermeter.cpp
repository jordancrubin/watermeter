/*
  Watermeter.cpp - Control Library for reed switch pulse actuated water meters 
  This one designed around the DAE Water Meter Model V-100P
  Most of these meters operate in the same manner, the difference
  can be adjusted as parameters in the instance creation.  Saving works
  With either SPIFFS or SDFS as a compile time parameter.

  https://www.youtube.com/c/jordanrubin6502
  2020 - 2022 Jordan Rubin.
*/

#include <Arduino.h> // Required for Platform.io
#include <Watermeter.h>

// Choose only one below filesystem....... USE_SPIFFS or USE_SD
#define USE_SPIFFS 1
//#define USE_SD 1
////////////You Must choose one, and not both if saving to FS!
#if (USE_SPIFFS)
#include <SPIFFS.h>  
#endif
#if(USE_SD)
#include <SD.h>
#include <FS.h>
#endif

int meterSignal;
bool gallon = 0;
bool update;
bool debug = false;
long lastDebounce = 0;    // Holds last debounce
long debounceDelay;       // Between re-polling
long lastUpdate = 0;
double meter = 0;
float increment;
int saveint;    
bool useFilesys;

void IRAM_ATTR respondInterrupt();

//CONSTRUCTOR -----------------------------------------------------------
//WATERMETER::WATERMETER(int signalGPIOpin, bool useInternalPullups,char measure, long dbounce ,bool useFS, float incr, int saveInterval, bool debug)
WATERMETER::WATERMETER(int signalGPIOpin, bool useInternalPullups,char measure, long dbounce ,bool useFS, float incr, int saveInterval, bool debugger)
{
  //----------------- initialize initial parameters --------------------------]
    if (useInternalPullups){
      pinMode(signalGPIOpin, INPUT_PULLUP);
    }
    else {
      pinMode(signalGPIOpin, INPUT);
    }
    if (measure == 'g'){gallon = 1;}
    attachInterrupt(signalGPIOpin, respondInterrupt, FALLING);
    meterSignal = signalGPIOpin;
    increment = incr;
    debug = debugger;
    saveint = saveInterval * 1000;
    debounceDelay = dbounce;
    useFilesys = useFS;
}
// ----------------------------------------------------------------------------]

// FUNCTION - [updated] - [Checks and responds to the update flag--------------]
bool WATERMETER::updated(void){
  if (update){
  if (!useFilesys){update = 0; return 1;}  
    if(saveint == 0){return 1;}
    if ((millis() - lastUpdate) > saveint) { 
      writeFile();
      lastUpdate = millis(); 
      update = 0;
    }
    return 1;
  }
  return 0;
} 
// ----------------------------------------------------------------------------]

// FUNCTION - [getDebounce] - [returns the Debounce delay in mS----------------]
int WATERMETER::getDebounce(void){
  return debounceDelay;
} 
// ----------------------------------------------------------------------------]

// FUNCTION - [initFilesys] - [Starts up the Meter File system and test--------]
const char* WATERMETER::initFilesys(void){
  if (useFilesys){
#if (USE_SPIFFS)
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
#endif 
#if (USE_SD)   
      if(!SD.begin()){ 
        return "ERR";
      }
      //DELETE FOR TESTING
      //SD.remove("/meter/meter.val");       
      File root = SD.open("/meter");
      if(!root.isDirectory()){
        SD.mkdir("/meter");
      }  
      root.close();
      if(!SD.exists("/meter/meter.val")){ 
        writeFile();
        return "NEW";
      }
      else {
        bool reset = readFile();
        if (reset){return "RESET";}
      }
#endif   
  }
  else {
    return "NONE";
  }
  return "OK";
}
// ----------------------------------------------------------------------------]

// FUNCTION - [readFile] - [Opens and reads meter file into meter value--------]
bool WATERMETER::readFile(){
  if (!useFilesys){
    if(debug){Serial.print("No FS Enabled"); 
      return 1;
    } 
  }   
  bool resetalarm=0;
#if(USE_SPIFFS)
 File meterfile = SPIFFS.open("/meter.val","r");
#endif
#if(USE_SD)  
  File meterfile = SD.open("/meter/meter.val");
#endif   
  char test[20];
  int i=0;
  while (meterfile.available()) {
    char B = meterfile.read();
    test[i] = B;
    i++;
    if (i >=15) {
      resetalarm=1;
#if(USE_SPIFFS)      
      SPIFFS.remove("/meter.val");
#endif   
 #if(USE_SD) 
      SD.remove("/meter/meter.val");
 #endif  
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
  writeFile();
} 
// ----------------------------------------------------------------------------]

// FUNCTION - [writeFile] - [Opens or creates meter file, writes value---------]
void WATERMETER::writeFile(){
#if(USE_SPIFFS)
  File  meterfile = SPIFFS.open("/meter.val","w");
#endif
#if(USE_SD)  
  File meterfile = SD.open("/meter/meter.val",FILE_WRITE);
#endif
  char convert[10];
  sprintf(convert, "%6.2f", meter);
  meterfile.print(convert);
  if(debug){Serial.print("meter writesd: ");Serial.println(convert);}
  meterfile.close();
}
// ----------------------------------------------------------------------------]
