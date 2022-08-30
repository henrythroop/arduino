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
  int SW = A2;

  int xPosition = 0;
  int yPosition = 0;
  int SW_state = 0;
  int mapX = 0;
  int mapY = 0;
  int isButton = 0;

/* Set up song stuff */
  
int speakerPin = 9; // Buzzer pin
int length = 28; // the number of notes
char notes[] = "GGAGcB GGAGdc GGxecBA yyecdc";
int beats[] = {2,2,8,8,8,16,1,2,2,8,8,8,16,1,2,2,8,8,8,8,16,1,2,2,8,8,8,16};
int tempo = 200;// time delay between notes 


void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
     digitalWrite(speakerPin, HIGH);
     delayMicroseconds(tone);
     digitalWrite(speakerPin, LOW);
     delayMicroseconds(tone);
  }
}

void playSong(){
  for (int i = 0; i < length; i++) {
   if (notes[i] == ' ') {
     delay(beats[i] * tempo); // delay between notes
   } else {
     playNote(notes[i], beats[i] * tempo);
   }
   // time delay between notes
   delay(tempo);
  }
}

void playNote(char note, int duration) {
  char names[] = {'C', 'D', 'E', 'F', 'G', 'A', 'B',           
                 'c', 'd', 'e', 'f', 'g', 'a', 'b',
                 'x', 'y' };

  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014,
                 956,  834,  765,  593,  468,  346,  224,
                 655 , 715 };

  int SPEE = 5;
  // play the tone corresponding to the note name

  for (int i = 0; i < 17; i++) {

     if (names[i] == note) {
      int newduration = duration/SPEE;
       playTone(tones[i], newduration);
     }
  }
}


void setup()

{

/* Set up the motor stuff */

  Serial.begin(9600);

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP); 

  int speed = 255;  /* Must be at least 150. Otherwise, voltage is too low and inconsistent */
  
  motorLeftFront.setSpeed(speed);
  motorLeftRear.setSpeed(speed);
  motorRightFront.setSpeed(speed);
  motorRightRear.setSpeed(speed);

  pinMode(speakerPin, OUTPUT);

}
  
void loop()

{

/* Read Joystick */
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  isButton = digitalRead(SW);
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
    else if (isButton == 0) { /* Play song on button down */
    Serial.println("Play Song!");
    playSong();
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
