//----------------------------------------------------------------------------
// MS583702BA01.h 
// Version      : V1.0 
// Description  : Library for the MS583702BA01 temperature and pressure sensor
//----------------------------------------------------------------------------
// Author       : Corentin Seydoux
// Date         : 05.03.2022 
// ---------------------------------------------------------------------------

#ifndef __MS583702BA01__
#define __MS583702BA01__

#include <Arduino.h>
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

        int address = 0b11101100; //I2c address

        void Init(int address); 
        
        void getMeasurements(int tempOSR, int pressureOSR);
};

#endif //__MS583702BA01__
