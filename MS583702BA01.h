//----------------------------------------------------------------------------
// MS583702BA01.h 
// Version      : V1.0 
// Description  : Library for the MS583702BA01 temperature and pressure sensor
//----------------------------------------------------------------------------
// Author       : Corentin Seydoux
// Date         : 05.03.2022 
// ---------------------------------------------------------------------------

//MS583702BA01.h
#ifndef _MS583702BA01
#define _MS583702BA01

#include <Wire.h>

#define port Wire2

class MS583702BA01{
    private:
        unsigned int calibrationData[6];
        //calibrationData[1] :  Pressure sensitivity    
        //calibrationData[2] :  Pressure offset
        //calibrationData[3] :  Temperature coefficient of pressure sensitivity
        //calibrationData[4] :  Temperature coefficient of pressure offset 
        //calibrationData[5] :  Reference temperature 
        //calibrationData[6] :  Temperature coefficient of the temperature

        unsigned int D1; //Digital pressure value 
        unsigned int D1; //Digital temperature value

        int dT; //Difference between actual and reference temperature

        double OFF; //Pressure offset at actual temperature

        double SENS; //Pressure sensitivity at actual temperature

        int getSensorValue(int sensor, int OSR);
    public:
        int temp; //Actual temperature
        int pressure; //Temperature compensated pressure

        int address; //I2c address

        void Init(int address); 
        
        void getMeasurements(int tempOSR, int pressureOSR);
};


//----------------------------------------------------------------------------
// Function     : Init()
//----------------------------------------------------------------------------
// Author       : Corentin Seydoux
// Date         : 05.03.2022 
// ---------------------------------------------------------------------------
// Description  : Initialize the sensor by resetting it and loading the
//                calibration data
// Parameters   : none
// Return       : none
//----------------------------------------------------------------------------
void Init(){
    //RESET SEQUENCE
    port.beginTransmission(address);
    port.write(0x1E); //reset
    port.endTransmission();
    
    //Read calibration data from PROM
    for(int idx = 0; idx < 5; idx++){
    port.beginTransmission(address);
    port.write(0b10100000 + ((idx+1)<<1)); //PROM read to adress idx+1
    port.beginTransmission(address + 1);
    byte CxMsb = port.read();
    byte CxLsb = port.read();

    calibrationData[idx] = CxMsb<<8 + CxLsb;
    }
}


//----------------------------------------------------------------------------
// Function     : getMeasurements()
//----------------------------------------------------------------------------
// Author       : Corentin Seydoux
// Date         : 05.03.2022 
// ---------------------------------------------------------------------------
// Description  : Get the compensated temperature and pressure measurements
//                
// Parameters   : tempOSR : temperature resolution
//                if OSR = 256 -> resolution = 0.012°
//                if OSR = 512 -> resolution = 0.009°C
//                if OSR = 1024 -> resolution = 0.006°C
//                if OSR = 2048 -> resolution = 0.004°C
//                if OSR = 4096 -> resolution = 0.003°C
//                if OSR = 8192 -> resolution = 0.002°C
//
//                pressureOSR : pressure resolution
//                if OSR = 256 -> resolution = 0.11mbar
//                if OSR = 512 -> resolution = 0.062mbar
//                if OSR = 1024 -> resolution = 0.039mbar
//                if OSR = 2048 -> resolution = 0.028mbar
//                if OSR = 4096 -> resolution = 0.021mbar
//                if OSR = 8192 -> resolution = 0.016mbar
// Return       : none
//----------------------------------------------------------------------------
void getMeasurements(int tempOSR, int pressureOSR){
    //Read digital pressure value    
    D1 = getSensorValue(0, pressureOSR);

    //Read digital temperature value    
    D2 = getSensorValue(1, tempOSR); 

    //Calculate difference between actual and reference temperature
    dT = D2 - (calibrationData[4] * 256);

    //Calculate actual temperature
    temp = 2000 + (dT * calibrationData[5] / 8388608);

    //Calculate pressure offset at actual temperature
    OFF = (calibrationData[1] * 131072) + ((calibrationData[3] * dT) / 64);

    //Calculate sensitivity at actual temperature
    SENS = (calibrationData[0] * 65536) + ((calibrationData[2] * dT) / 128);

    //Calculate temperature compensated pressure
    pressure = (((D1 * SENS / 2097152) - OFF) / 32768);
}

//----------------------------------------------------------------------------
// Function     : getSensorValue()
//----------------------------------------------------------------------------
// Author       : Corentin Seydoux
// Date         : 05.03.2022 
// ---------------------------------------------------------------------------
// Description  : Get the measurement of the pressure or temperature
//                
// Parameters   : sensor : 
//                if sensor = 0 -> get pressure value
//                if sensor = 1 -> get temperature value
//
//                OSR : resolution of the measurement
// Return       : Return the value of the measurement
//----------------------------------------------------------------------------
int getSensorValue(int sensor, int OSR){
    //Read digital value of the sensor
    //if sensor = 0 -> get pressure value
    //if sensor = 1 -> get temperature value
    port.beginTransmission(address);
    
    switch(OSR){
        case 256 :
            port.write(0x40 + (sensor*0x10));
            break;
        case 512 :
            port.write(0x42 + (sensor*0x10));
            break;
        case 1024 :
            port.write(0x44 + (sensor*0x10));
            break;
        case 2048 :
            port.write(0x46 + (sensor*0x10));
            break;
        case 4096 :
            port.write(0x48 + (sensor*0x10));
            break;
        case 8192 :
            port.write(0x4A + (sensor*0x10));
            break;
    }
    while (Wire.available()) {} //Wait for the conversion to finish
                                
    port.endTransmission();

    //ADC read
    port.beginTransmission(address);
    port.write(0x00);
    port.endTransmission();

    port.beginTransmission(address + 1);

    int tabAdcValue[3];
    for(int idx = 0; idx <= 2; idx++){
        tabAdcValue[idx] = port.read();
    } 
    
    return((tabAdcValue[0]<<16) + (tabAdcValue[1]<<8) + tabAdcValue[2]);    
} 

#endif //MS583702BA01.h
