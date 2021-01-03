#include "Position.h"

Position::Position(float Latdeg,float Latmin, float Latsec, float Longdeg, float Longmin, float Longsec)
{
    Lat_ = DMS2DEC(Latdeg,Latmin,Latsec);
    Long_ = DMS2DEC(Longdeg,Longmin,Longsec);
};

void Position::radec2altaz(Timestamp Time)
{
    float RA_Deg = TargetRA_*15.0;
    float Dec_Rad = radians(TargetDec_);
    float Lat_Rad = radians(Lat_);
    float LST = 100.46 + 0.985647 * Time.CurrentDaySinceJ2000() + Long_ + 15 * Time.HourOfDay();

    if (LST > 360.0)
    {
        LST = LST - (360 * floor(LST/360));
    }
    else if (LST < 0)
    {
        LST = LST + (360 * abs(floor(LST/360)));
    }
    float HA = LST - RA_Deg;
    if (HA < 0)
    {
        HA = HA + 360;
    }
    float HA_Rad = radians(HA);
    //*Convert HA?Dec to ALt/AZ
    //* ALT
    float ALT_Rad = asin(sin(Dec_Rad)*sin(Lat_Rad)+cos(Dec_Rad)*cos(Lat_Rad)*cos(HA_Rad));
    TargetAlt_ = degrees(ALT_Rad);
    //* AZ
    float AZ_Rad = acos((sin(Dec_Rad)-sin(ALT_Rad)*sin(Lat_Rad))/(cos(ALT_Rad)*cos(Lat_Rad)));
    if (sin(HA_Rad)>0)
    {
        TargetAz_ = 360 - degrees(AZ_Rad);
    }
    else
    {
        TargetAz_ = degrees(AZ_Rad);
    }
};

void Position::setCurrentPosition(float CurrentAz, float CurrentAlt)
{
    CurrentAz_ = CurrentAz;
    CurrentAlt_ = CurrentAlt;
};

void Position::setTarget(float TargetRA, float TargetDec,Timestamp Time)
{
    TargetRA_ = TargetRA;
    TargetDec_ = TargetDec;
    radec2altaz(Time);
};

void Position::setTargetfromText(String Msg,Timestamp Time)
{
    TargetRA_ = Msg.substring(8,15).toFloat();
    TargetDec_ = Msg.substring(20).toFloat();
    radec2altaz(Time);
};

void Position::setParkTarget()
{
    TargetAz_ = 280.0;
    TargetAlt_ = 0.0;
};

void Position::printTargetLoc()
{
    Serial.println("CurrentTarget: ");
    Serial.print("Az: ");
    Serial.print(TargetAz_);
    Serial.print("\tAlt: ");
    Serial.print(TargetAlt_);
    Serial.println("");
};

void Position::printCurrentLoc()
{
    Serial.println("CurrentLocation: ");
    Serial.print("Az: ");
    Serial.print(CurrentAz_);
    Serial.print("\tAlt: ");
    Serial.print(CurrentAlt_);
    Serial.println("");
};

bool Position::validPos()
{
    if ((CurrentAlt_ != NULL) & (CurrentAz_ != NULL))
    {
        return true;
    }
    else
    {
        return false;
    }
    
};

float Position::getTargetAlt(Timestamp Time)
{
    radec2altaz(Time);
    return TargetAlt_;
};

float Position::getTargetAz(Timestamp Time)
{
    radec2altaz(Time);
    return TargetAz_;
};

float Position::getCurrentAlt()
{
    return CurrentAz_;
};

float Position::getCurrentAz()
{
    return CurrentAz_;
}