#include <Servo.h>

/* This is a test code that goes through a long loop */
/* and makes all of the motors run */
/* This works. */
/* From https://circuitdigest.com/microcontroller-projects/arduino-motor-driver-shield-pcb */
/* HBT 24-Aug-2022 */

Servo myservo;

#define MR 2
#define M2PWM 3
#define SH 4
#define M4PWM 5
#define M3PWM 6
#define OE 7
#define DS 8
#define SERVO1 9
#define SERVO2 10
#define M1PWM 11
#define ST 12
#define M1 0
#define M2 2
#define M3 4
#define M4 6
#define STOP 0
#define CW 1
#define CCW 2 


char pAction=0x00;

void setup()

{

  Serial.begin(9600);
  pinMode(MR, OUTPUT);
  pinMode(M2PWM, OUTPUT);
  pinMode(SH, OUTPUT);
  pinMode(M4PWM, OUTPUT);
  pinMode(M3PWM, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(DS, OUTPUT);
  pinMode(M1PWM, OUTPUT);
  pinMode(ST, OUTPUT);
  digitalWrite(M1PWM, HIGH);
  digitalWrite(M2PWM, HIGH);
  digitalWrite(M3PWM, HIGH);

  digitalWrite(M4PWM, HIGH);

  digitalWrite(MR, HIGH);

  digitalWrite(OE, LOW);

  myservo.attach(SERVO1);

  myservo.write(0);

}

void loop()

{

  DriveMotor(M1,CW);
  DriveMotor(M2,CCW);
  DriveMotor(M2,CW);
  DriveMotor(M3,CW);
  DriveMotor(M4,CW);
  myservo.write(0);

  delay(5000);
  DriveMotor(M1,STOP);
  DriveMotor(M2,STOP);
  DriveMotor(M3,STOP);
  DriveMotor(M4,STOP);

  delay(1000);

  DriveMotor(M1,CCW);
  DriveMotor(M2,CCW);
  DriveMotor(M3,CCW);
  DriveMotor(M4,CCW);
  myservo.write(90);

  delay(5000);

      DriveMotor(M1,STOP);
  DriveMotor(M2,STOP);
  DriveMotor(M3,STOP);
  DriveMotor(M4,STOP);

  delay(1000);

  DriveMotor(M1,CW);
  DriveMotor(M2,CCW);
  DriveMotor(M3,CW);
  DriveMotor(M4,CCW);
  myservo.write(180);

  delay(5000);

      DriveMotor(M1,STOP);
  DriveMotor(M2,STOP);
  DriveMotor(M3,STOP);
  DriveMotor(M4,STOP);

  delay(1000);

  DriveMotor(M1,CCW);
  DriveMotor(M2,CW);
  DriveMotor(M3,CCW);
  DriveMotor(M4,CW);
  myservo.write(90);

  delay(5000);

      DriveMotor(M1,STOP);

  DriveMotor(M2,STOP);

  DriveMotor(M3,STOP);

  DriveMotor(M4,STOP);

  delay(1000);

  DriveMotor(M1,STOP);

  DriveMotor(M2,CW);

  DriveMotor(M3,CCW);

  DriveMotor(M4,CW);

  myservo.write(0);

  delay(5000);

      DriveMotor(M1,STOP);

  DriveMotor(M2,STOP);

  DriveMotor(M3,STOP);

  DriveMotor(M4,STOP);

  delay(1000);

  DriveMotor(M1,STOP);

  DriveMotor(M2,STOP);

  DriveMotor(M3,CW);

  DriveMotor(M4,CCW);

  myservo.write(90);

  delay(5000);

    DriveMotor(M1,STOP);

  DriveMotor(M2,STOP);

  DriveMotor(M3,STOP);

  DriveMotor(M4,STOP);

  delay(1000);

  DriveMotor(M1,STOP);

  DriveMotor(M2,STOP);

  DriveMotor(M3,STOP);

  DriveMotor(M4,CCW);

  myservo.write(180);

  delay(5000);

  DriveMotor(M1,STOP);

  DriveMotor(M2,STOP);

  DriveMotor(M3,STOP);

  DriveMotor(M4,STOP);

  myservo.write(90);

  delay(5000);

}

int Action=0;

void DriveMotor(int Motor, int Dir)

{

  //  Serial.print("Motor :");

  //  Serial.println(Motor, HEX);

  //  Serial.print("Action:");

  //  Serial.println(Action,HEX);

    if(Dir == CW)

    {

      Action|=(1<<Motor);

      Action&=~(1<<Motor+1);

    }

    else if(Dir == CCW)

    {

      Action&=~(1<<Motor);

      Action|=(1<<Motor+1);

    }

    else

    {

            Action&=~(1<<Motor);

            Action&=~(1<<Motor+1);

    }

    Serial.print("Action:");

    Serial.println(Action, HEX);

   // delay(2000);

    

  for (int i = 0; i < 8; i++)

  {

    if ((Action << i) & 0x80)

      digitalWrite(DS, HIGH);

    else

      digitalWrite(DS, LOW);

    digitalWrite(SH, HIGH);

    delay(1);

    digitalWrite(SH, LOW);

  }

  digitalWrite(ST, HIGH);

  delay(1);

  digitalWrite(ST, LOW);

  pAction=Action;

}
