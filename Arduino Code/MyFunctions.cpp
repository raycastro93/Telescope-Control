#include "MyFunctions.h"

/*
COORD ParseSlew(String Message)
{
    //Message format is "SLEW:RA:+###.####,DEC:+##.####"
    //Serial.println(Message.substring(8,15));
    //Serial.println(Message.substring(20));
    COORD COORDs;
    sscanf(Message.c_str(),"SLEW:RA:%06.4f,DEC:%+06.4f",COORDs.RA,COORDs.Dec);
    //COORDs.RA = Message.substring(8,15).toFloat();
    //COORDs.Dec = Message.substring(20).toFloat();
    return COORDs;
}
*/
double ConstrainAngle180(double x)
{
    x = fmod(x+180,360);
    if (x<0)
    {
        x+=360;
    }
    if (x == 0)
    {
        x = 360;
    }
    return x - 180;
}

double ConstrainAngle360(double x)
{
    x = fmod(x,360);
    if (x<0){
        x += 360;
    }
    return x;
}

float DMS2DEC(float Deg, float Min, float Sec)
{
    return Deg + Min/60.0 + Sec/3600.0;
}