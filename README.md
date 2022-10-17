# Watermeter
 The advanced watermeter project
  This one designed around the DAE Water Meter Model V-100P
  Most of these meters operate in the same manner, the difference
  can be adjusted as parameters in the instance creation. Saving works
  With either SPIFFS or SDFS as a compile time parameter.

  This was created as part of a larger irrigation project found at.
  https://www.youtube.com/c/jordanrubin6502


//WATERMETER::WATERMETER(int signalGPIOpin, bool useInternalPullups,char measure, long dbounce ,bool useFS, float incr, int saveInterval, bool debug)

Using GPIO 15                            [15]

Using internal pullup resistors          [true]

This meter is designed as gallons        [g]

Debounce delay in ms                     [100] 

Use File System to save meter to filesys [true]          

The value of measurment added each time  [.1] i.e. .1 gallons per pulse

The save interval to filesystem in seconds [30] 0 disables autosave  

WATERMETER thisMeter(15,true,'g',100,true,.1,true,30);

  When saving to the filesystem, one type of filesystem must be uncommented in Watermeter.cpp USE_SPIFFS or USE_SD.
  You MUST choose one, and ONLY one. Software arrives defaulted to USE_SPIFFS.

  If USE_SD card is choosen the programme will expect a fat32 formatted SD card installed in the unit.
  It uses the default connections.  SPIFFS hase been completely removed as of this version, given the
  constant barrage of read and saves to the ESP32 thousands of times over is unhealthy for the device. 
  The save interval further limits constant writes to the SD card by limiting updates to the interval. 
  
  CS	  -> GPIO 5
  MOSI	-> GPIO 23
  CLK	  -> GPIO 18
  MISO	-> GPIO 19


Internal pullup resistors are available on the ESP32 but may not be on the Arduino or certain models requiring external resistors be provided for GPIO pins.  Optimal debounce delay will need to be tested comparing the program to the physical meter accuracy.  The SD card initialization has autocorruption detection, should the file be found to be corrupted on init, it will pull in the meter value from the parameter.  Verbose output is usefull for testing.

This latest version attempts to reduce the footprint while making some improvment as I want all wasted space removed.  

Verbose serial output is now religated to the end user not the library, allowing for better user control and about .5K reduction.

citing an example we see different ways to call init and how to process it.

  Print the result codes out directly as the init is called
  Serial.println(thisMeter.initFilesys());


  Or assign the result to a const char and process it after function execution
  const char * result = thisMeter.initFilesys();
  Serial.println(result);


  Or use a methodology to test success directly
  if (strcmp(thisMeter.initFilesys(),"OK")==0){
    Serial.println("SPIFFS STARTED");
  }

  The codes are
OK -    Everything worked as expected
ERR-    The SPIFFS system was corrupted and couldnt be mounted
NEW-    It found no Metre file and created a new one
RESET-  The file had data errors and was re-initialized
NONE-   The function was executed but the object has no param for SPIFFS support





