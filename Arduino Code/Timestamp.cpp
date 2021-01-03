#include "Timestamp.h"

Timestamp::Timestamp()
{
    year_ = 0;
    day_ = 0;
    month_ = 0;
    hour_ = 10000;
    min_ = 10000;
    sec_ = 10000.0;
};
void Timestamp::fromText(String msg)
{
    year_ = msg.substring(5,9).toInt();    
    month_ = msg.substring(9,11).toInt();
    day_ = msg.substring(11,13).toInt();
    hour_ = msg.substring(13,15).toInt();
    min_ = msg.substring(15,17).toInt();
    sec_ = msg.substring(17).toFloat();
};
void Timestamp::setDate(unsigned int year, unsigned int day, unsigned int month)
{
    year_ = year;
    day_ = day;
    month_ = month;
};
void Timestamp::setTime(unsigned int hour, unsigned int min, float sec)
{
    hour_ = hour;
    min_ = min;
    sec_ = sec;
};
float Timestamp::HourOfDay()
{
    float HoD = float(hour_)+float(min_)/60.0+sec_/3600.0;
    return HoD;
};
unsigned int Timestamp::getYear()
{
    return year_;
};
unsigned int Timestamp::getDay()
{
    return day_;
};
unsigned int Timestamp::getMonth()
{
    return month_;
};
void Timestamp::printCurrentTime()
{
    char buffer[50];
    snprintf(buffer,50,"%04d/%02d/%02d,%02d:%02d:%04.2f",year_,month_,day_,hour_,min_,sec_);
    Serial.println(buffer);
}
bool Timestamp::validTime()
{
    if((year_ != 0) & (day_!=0) & (month_ !=0) & (hour_!=10000) & (min_!=10000) & (sec_!=10000.0))
    {
        return true;
    }
    else
    {
        return false;
    }
    
}

unsigned int Timestamp::DayOfYear()
{
    if(year_ % 4 == 0)
    {
        if(year_ % 100 == 0)
        {
            if(year_ % 400 == 0)
            {
                return leapyearcount[month_-1];
            }
        
            else
            {
                return yearcount[month_-1];
            }
        }
        else
        {
            return leapyearcount[month_-1];
        }
    }
    else
    {
        return yearcount[month_-1];
    }
    
};

float Timestamp::CurrentDaySinceJ2000()
{
    return DayOfYear() + day_ + HourOfDay()/24.0 + DaysSinceJ2000[year_-1998];
}