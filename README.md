# Watermeter
 The advanced watermeter project
  This one designed around the DAE Water Meter Model V-100P
  Most of these meters operate in the same manner, the difference
  can be adjusted as parameters in the instance creation.

  This was created as part of a larger irrigation project found at.
  https://www.youtube.com/c/jordanrubin6502


WATERMETER WATERMETER(SignalGPIOpin,useInternalPullupResistor,measure[g|l],debounce,spiffs,metervalue,verbose)

Using GPIO 15                            [15]

Using internal pullup resistors          [true]

This meter is designed as gallons        [g]

Debounce delay in ms                     [100] 

Use SPIFFS to autosave meter to filesys  [true]          

The value of measurment added each time  [.1] i.e. .1 gallons per pulse

Verbose output to the Serial console     [true] 

WATERMETER thisMeter(15,true,'g',100,true,.1,true);

Internal pullup resistors are available on the ESP32 but may not be on the Arduino or certain models requiring external resistors be provided for GPIO pins.  Optimal debounce delay will need to be tested comparing the program to the physical meter accuracy.  The SPIFFS initialization has autocorruption detection, should the file be found to be corrupted on init, it will pull in the meter value from the parameter.  Verbose output is usefill for testing.