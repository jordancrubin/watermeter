# Watermeter
 The advanced watermeter project
  This one designed around the DAE Water Meter Model V-100P
  Most of these meters operate in the same manner, the difference
  can be adjusted as parameters in the instance creation.

  This was created as part of a larger irrigation project found at.
  https://www.youtube.com/c/jordanrubin6502


//WATERMETER WATERMETER(SignalGPIOpin,useInternalPullupResistor,measure[g|l],metervalue)
// Using GPIO 15                            [15]
// Using internal pullup resistors          [true]
// This meter is designed as gallons        [g]
// Debounce delay in ms                     [100] 
// Use SPIFFS to autosave meter to filesys  [true]          
// The value of measurment added each time  [.1] i.e. .1 gallons per pulse
// Verbose output to the Serial console     [true] 
WATERMETER thisMeter(15,true,'g',100,true,.1,true);