/*
  Watermeter.h - Control Library for reed switch pulse actuated water meters 
  This one designed around the DAE Water Meter Model V-100P
  Most of these meters operate in the same manner, the difference
  can be adjusted as parameters in the instance creation.
  https://www.youtube.com/c/jordanrubin6502
  2020 Jordan Rubin.
*/

// ensure this library description is only included once
#ifndef Watermeter_h
#define Watermeter_h

// library interface description
class WATERMETER
{
  ////////////// user-accessible "public" interface
  //WATERMETER(SignalGPIOpin,useInternalPullupResistor,measure[g|l],dbouncedelay,useSPIFFS,increment,verbose)
  public:
    WATERMETER(int,bool,char,long,bool,float,bool);
    bool checkUpdate(void);
    int getDebounce(void);
    const char* initFilesys(void);
    bool isUpdated(void); 
    double readOut(void); 
    double readOut(char);
    void setMeter(double);
    void setDebounce(int);
    
//  ////////////// library-accessible "private" interface
  private:
    int value;
    void readFile(void);
    void writeFile(void);
};

#endif