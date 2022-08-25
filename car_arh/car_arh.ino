#include <AFMotor.h>  /* Adafruit 4-motor shield driver */
                      /* Tis driver defines FORWARD, BACKWARD, RELEASE, etc. */

/* This is HBT code to drive car with joystick */
/* HBT + ARH 24-Aug-2022 */

/* Set up motor stuff */

AF_DCMotor motorLeftRear(3);  /* Figure these out bt testing w/ af_motor_test.io */
AF_DCMotor motorLeftFront(4);
AF_DCMotor motorRightFront(1);
AF_DCMotor motorRightRear(2);

/* Set up the joystick stuff */

  int VRx = A0;
  int VRy = A1;
  int SW = 2;

  int xPosition = 0;
  int yPosition = 0;
  int SW_state = 0;
  int mapX = 0;
  int mapY = 0;

void setup()

{

/* Set up the motor stuff */

  Serial.begin(9600);

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP); 

  int speed = 255;
  
  motorLeftFront.setSpeed(speed);
  motorLeftRear.setSpeed(speed);
  motorRightFront.setSpeed(speed);
  motorRightRear.setSpeed(speed);

}
  
void loop()

{

/* Read Joystick */
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);
  mapX = map(xPosition, 0, 1023, -512, 512);
  mapY = map(yPosition, 0, 1023, -512, 512);

  bool isXCenter = (mapX > -400 && mapX < 400);
  bool isXLeft   = (mapX >  400);
  bool isXRight  = (mapX < -400);
  
  bool isYCenter = (mapY > -400 && mapY < 400);
  bool isYBack   = (mapY < -400);
  bool isYFront  = (mapY >  400);

  if (isXLeft && isYFront) { /* Forward + Left */
    Serial.println("Forward Left");
    motorRightFront.run(FORWARD);
    motorRightRear.run(FORWARD);
    motorLeftFront.run(RELEASE);
    motorLeftRear.run(RELEASE);
    
    delay(100);
  } 
  else if (isXRight && isYFront) { /* Forward + Right */
    Serial.println("Forward Right");
    motorLeftFront.run(FORWARD);
    motorLeftRear.run(FORWARD);
    motorRightFront.run(RELEASE);
    motorRightRear.run(RELEASE);
    
    delay(100);
  } 
  else if (isXRight && isYBack) { /* Backward + Right */
    Serial.println("Backward Right");
    motorLeftFront.run(RELEASE);
    motorLeftRear.run(RELEASE);    
    motorRightFront.run(BACKWARD);
    motorRightRear.run(BACKWARD);
    delay(100);
  } 
  else if (isXLeft && isYBack) { /* Backward + Left */
    Serial.println("Backward Left");
    motorRightFront.run(RELEASE);
    motorRightRear.run(RELEASE);    
    motorLeftFront.run(BACKWARD);
    motorLeftRear.run(BACKWARD);
    delay(100);
  } 
  else if (isXCenter && isYFront) { /* Drive Forward */
    Serial.println("Forward");
    motorLeftFront.run(FORWARD);
    motorLeftRear.run(FORWARD);    
    motorRightFront.run(FORWARD);
    motorRightRear.run(FORWARD);
    delay(100);
  }
    else if (isXLeft && isYCenter) { /* Turn Left */
    Serial.println("Left");
    motorRightFront.run(FORWARD); /* OK now */
    motorRightRear.run(FORWARD);
    motorLeftFront.run(BACKWARD);
    motorLeftRear.run(BACKWARD);
    delay(100);
  }
    else if (isXRight && isYCenter) { /* Turn Right */
    Serial.println("Right");
    motorRightFront.run(BACKWARD); /* OK now */
    motorRightRear.run(BACKWARD);
    motorLeftFront.run(FORWARD);
    motorLeftRear.run(FORWARD);
    delay(100);
  }
    else if (isXCenter && isYBack) { /* Drive Backward */
    Serial.println("Backward");
    motorLeftFront.run(BACKWARD);
    motorLeftRear.run(BACKWARD);    
    motorRightFront.run(BACKWARD);
    motorRightRear.run(BACKWARD);
    delay(100);
  }
  else {                           /* If at center position */
    Serial.println("Stop");
    motorLeftFront.run(RELEASE);
    motorLeftRear.run(RELEASE);    
    motorRightFront.run(RELEASE);
    motorRightRear.run(RELEASE);
  }
}
