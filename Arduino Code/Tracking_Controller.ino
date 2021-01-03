#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "MyFunctions.h"
#include <utility/quaternion.h>
#include "Timestamp.h"
#include "Position.h"

//* BNO055 I2C Init
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//* BNO055 status variables
uint8_t system_stat = 0;
uint8_t gyro_stat = 0;
uint8_t accel_stat = 0;
uint8_t mag_stat = 0;

//* Motor Shield Init
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *AZ_Motor = AFMS.getStepper(200,1);
Adafruit_StepperMotor *ALT_Motor = AFMS.getStepper(200,2);

//* Wrapper functions for Accel Stepper library
void AZCCWstep()
{
    //* Overall AZ Movement in CCW (negative) Direction
    AZ_Motor->onestep(FORWARD, DOUBLE);
}
void AZCWstep()
{
    //* Overall AZ Movement in CW (Positive) Direction
    AZ_Motor->onestep(BACKWARD, DOUBLE);
}
void ALTDownStep()
{
    //* Overall ALT Movement to piviot down
    ALT_Motor->onestep(FORWARD, DOUBLE);
}
void ALTUpStep()
{
    //* Overall ALT Movement to piviot up
    ALT_Motor->onestep(BACKWARD,DOUBLE);
}

//* Accel Stepper Functions
AccelStepper AZStepper(AZCWstep,AZCCWstep);
AccelStepper ALTStepper(ALTUpStep,ALTDownStep);

//* Melbourne,FL Lat/Long
Position Pos(28,3.0,38.88,-80,-40,-19.93);
//const float Latitude = 28+3.0/60+38.88/3600;
//const float Longitude = -1*(80+40.0/60+19.93/3600);

//* Serial Comm Message
String cmd;

//* System Gear Ratios
const float AZGR = 236/1;
const float ALTGR = 128/1;

double AngleAzMove = 0;
double AngleAltMove = 0;

//* Offset between Magnetic North and True North
const double MagTrueOffset = 7.0;

bool Target = false;

//* Position Update Rate
long PospreviousMillis = 0;
long PosInterval = 500;

//* Variables for Debugging
const bool Debug = false;
long previousMillis = 0;
long interval = 1000;
unsigned long currentMillis = millis();

//* Class Init Variables
//Timestamp UTCTime = {0,0,0,0,0,0,0};
Timestamp UTCTime = Timestamp();
//Position CurrentPosition = {-1000.0,-1000.0};
//COORD CurrentTarget = {0,0,0,0};

void setup () {
    //*Serial Config
    Serial.begin(9600);

    if(!bno.begin())
    {
        //* Make sure BNO055 sensor is started and detected on startup
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while(1);
    }
    //* Set Initial Calibration values for BNO Sensor
    setCal();
    delay(1000);
    bno.setExtCrystalUse(true);
    delay(1000);
    bno.getCalibration(&system_stat,&gyro_stat,&accel_stat,&mag_stat);
    //Serial.println(system_stat);
    imu::Quaternion quat = bno.getQuat();
    imu::Vector<3> EulerAngle = quat.toEuler();
    Pos.setCurrentPosition(ConstrainAngle360(-1.0*degrees(EulerAngle.x()) - 90.0 - MagTrueOffset),degrees(EulerAngle.y()));
    Pos.printCurrentLoc();
    while (system_stat == 0)
    {
        Serial.println("Orientation unknown, Calibrate Sensor");
        displayCalStatus();
        bno.getCalibration(&system_stat,&gyro_stat,&accel_stat,&mag_stat);
        delay(1000);
        
    }
    Serial.println("Sensor Calibrated");
    delay(1000);
    
    //* Motor Shield Init
    AFMS.begin();
    AZStepper.setMaxSpeed(100.0);
    AZStepper.setAcceleration(100.0);
    
    ALTStepper.setMaxSpeed(100.0);
    ALTStepper.setAcceleration(100.0);

    Serial.flush();
}

void loop () 
{
    //* Update for Timing
    currentMillis = millis();

    //* Read serial and parse message
    if (Serial.available()>0)
    {
        cmd = Serial.readStringUntil('#');
        Serial.print("Received: ");
        Serial.println(cmd);
        if(cmd.substring(0,4) == "TIME")
        {
            // Message format is "TIME:YYYY/MM/DD,HH:MM:SS.00"
            UTCTime.fromText(cmd);// = ParseTime(cmd);
            if (Debug)
            {
                //* Print formated values to ensure read correctly while in debug mode
                UTCTime.printCurrentTime();
            }
        }
        else if(cmd.substring(0,4)=="SLEW")
        {
            //*Message format is "SLEW:RA:##.####,DEC:+##.####"
            //*Parse Coordinates
            Pos.setTargetfromText(cmd,UTCTime);
            if(Debug)
            {
                Pos.printTargetLoc();
            }
            //* Enable motor movements            
            Target=true;  
        }
        else if (cmd.substring(0,3)=="POS")
        {
            //* Send Current Position Information
            Pos.printCurrentLoc();
        }
        else if(cmd.substring(0,4)=="PARK")
        {
            //* Park Command
            Pos.setParkTarget();
            Target=true;
        }
        else if(cmd.substring(0,4)=="STOP")
        {
            Target = false;
        }
        else if (cmd.substring(0,8)=="SNSRSTAT")
        {
            displayCalStatus();
        }
    }

    //* Position Update Loop
    if (currentMillis - PospreviousMillis > PosInterval)
    {
        bno.getCalibration(&system_stat,&gyro_stat,&accel_stat,&mag_stat);
        displayCalStatus();
        imu::Quaternion quat = bno.getQuat();
        imu::Vector<3> EulerAngle = quat.toEuler();
        Pos.setCurrentPosition(ConstrainAngle360(-1.0*degrees(EulerAngle.x()) - 90.0 - MagTrueOffset),degrees(EulerAngle.y()));
        Pos.printCurrentLoc();
    }

    //* Read sensors, Error checking, and move motors
    if(!UTCTime.validTime() & Target)
    {
        Serial.println("No Time information currently known to determine RA/DEC positions");
    }
    else if (Target)
    {
        bno.getCalibration(&system_stat,&gyro_stat,&accel_stat,&mag_stat);
        if ((system_stat == 0) & !Pos.validPos())
        {
            Serial.println("Current Position is Unknown. Recalibrate sensor to determine current position");
        }
        else
        {
            //* Convert Target Coordinates
            //CurrentTarget = radec2altaz(CurrentTarget.RA,Latitude,Longitude,CurrentTarget.Dec,UTCTime);

            //* Determine Error
            // TODO need to apply code to move AZ in shortest direction
            AngleAzMove = ConstrainAngle180(Pos.getTargetAz(UTCTime) - Pos.getCurrentAz());
            AngleAltMove = Pos.getTargetAlt(UTCTime) - Pos.getCurrentAlt();

            //* Determine Movement for Stepper
            if ((1.8/AZGR)>abs(AngleAzMove)<0.5)
            {
                AZStepper.move(round(AngleAzMove*AZGR/1.8));
            }
            else
            {
                //* Release Motor if no movement is being applied
                AZ_Motor->release();
            }
            if ((1.8/ALTGR)>abs(AngleAltMove)<0.5)
            {
                ALTStepper.move(round(AngleAltMove*ALTGR/1.9));
            }
            else
            {
                //* Release Motor if no movement is being applied
                ALT_Motor->release();
            }
        }
    }     
    
    //* Move Steppers if movement is required
    ALTStepper.run();
    AZStepper.run();

    //* Constant stream of info at debug interval to serial monitor when enabled
    if (Debug){
        
        if(currentMillis - previousMillis > interval)
        {
            Pos.printTargetLoc();
            Serial.print("AzError: ");
            Serial.print(AngleAzMove,4);
            Serial.print("\tAltError: ");
            Serial.print(AngleAltMove, 4);
            Serial.println("");
        }
    }
}

//* BNO055 Calibration Value Function
void setCal()
{
    byte calData;
    bno.setMode(bno.OPERATION_MODE_CONFIG);    // Put into CONFIG_Mode
    delay(250);
    calData = bno.setCalvalARL(232);
    calData = bno.setCalvalARM(3);
    calData = bno.setCalvalMRL(69);
    calData = bno.setCalvalMRM(3);
    calData = bno.setCalvalAOXL(8);
    calData = bno.setCalvalAOXM(0);
    calData = bno.setCalvalAOYL(255);
    calData = bno.setCalvalAOYM(255);
    calData = bno.setCalvalAOZL(232);
    calData = bno.setCalvalAOZM(255);
    calData = bno.setCalvalMOXL(58);
    calData = bno.setCalvalMOXM(1);
    calData = bno.setCalvalMOYL(211);
    calData = bno.setCalvalMOYM(255);
    calData = bno.setCalvalMOZL(135);
    calData = bno.setCalvalMOZM(1);
    calData = bno.setCalvalGOXL(254);
    calData = bno.setCalvalGOXM(255);
    calData = bno.setCalvalGOYL(254);
    calData = bno.setCalvalGOYM(255);
    calData = bno.setCalvalGOZL(1);
    calData = bno.setCalvalGOZM(0);
    bno.setMode(bno.OPERATION_MODE_NDOF);    // Put into NDOF Mode
    delay(250);
}

void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  Serial.print("\t");
  if (!system)
  {
    Serial.print("! ");
  }

  /* Display the individual values */
  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.print(mag, DEC);
  Serial.println("");
}