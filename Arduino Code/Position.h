#ifndef Position_h
#define Position_h
#include "arduino.h"
#include "Timestamp.h"
#include "MyFunctions.h"

class Position
{
    private:
        float CurrentAz_;
        float CurrentAlt_;
        float TargetAz_;
        float TargetAlt_;
        float TargetRA_;
        float TargetDec_;
        float Lat_;
        float Long_;
        void radec2altaz(Timestamp Time);
    public:
        Position(float Latdeg,float Latmin, float Latsec, float Longdeg, float Longmin, float Longsec);
        void setCurrentPosition(float CurrentAZ, float CurrentAlt);
        void setTarget(float TargetRA, float TargetDec, Timestamp Time);
        void setTargetfromText(String Msg,Timestamp Time);
        void setParkTarget();
        void printTargetLoc();
        void printCurrentLoc();
        bool validPos();
        float getTargetAlt(Timestamp Time);
        float getTargetAz(Timestamp Time);
        float getCurrentAz();
        float getCurrentAlt();

};

#endif