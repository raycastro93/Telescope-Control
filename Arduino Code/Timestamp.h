#ifndef Timestamp_h
#define Timestamp_h
#include "arduino.h"

class Timestamp
{
    private:
        unsigned int year_;
        unsigned int day_;
        unsigned int month_;
        unsigned int hour_;
        unsigned int min_;
        float sec_;
        const float DaysSinceJ2000[25] = {-731.5,-366.5,-1.5,364.5,729.5,1094.5,1459.5,1825.5,2190.5,2555.5,2920.5,3286.5,3651.5,4016.5,4381.5,4747.5,5112.5,5477.5,5842.5,6208.5,6573.5,6938.5,7303.5,7669.5};
        const unsigned int yearcount[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
        const unsigned int leapyearcount[12] = {0,31,60,91,121,152,182,213,244,274,305,335};
    public:
        Timestamp();
        void fromText(String msg);
        void setDate(unsigned int year, unsigned int day, unsigned int month);
        void setTime(unsigned int hour, unsigned int min, float sec);
        float HourOfDay();
        unsigned int getYear();
        unsigned int getMonth();
        unsigned int getDay();
        void printCurrentTime();
        bool validTime();
        unsigned int DayOfYear();
        float CurrentDaySinceJ2000();
};

#endif