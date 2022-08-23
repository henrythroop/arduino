// Hot Wheels Race Timer
// v2.0
// by Ian Cook
// Check our YouTube channel at Cooks Projects
// https://www.youtube.com/channel/UC5M_udtMhAwnymLyAY5qAkw
// Released Feb 2021 under licence
// Please acknowledge me if you use any of my code

// A further refined program that allows dual function of constant speed measurement as well as the race timer is available for additional fee.
// See website links or https://www.emmaalexandercook.com/cooks-projects/p/race-timer for details.

// #include <I2CIO.h>
// #include <fastIO.h>
#include <Wire.h>
// #include <LCD.h>
//#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

#define POSITIVE true

// LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // double check your LCD address first using: i2C_scanner
LiquidCrystal_I2C lcd(0x27, 20, 4); // double check your LCD address first using: i2C_scanner
//NANO uses SDA on A4 and SCL on A5


#define DEBUG true //Use to toggle the debug function on/off
#define DEBUG_SENSORS false //Use to toggle the sensor debug function
#define COMP_OUTPUT true //Use this to send info to a connected computer via serial interface. Use false if 
                             // time is critical in detecting cars simultaneously
                             
#define SPEED_CRITICAL false //Set to TRUE to improve detection speeds of the SpeedClock() function so that simultaneous detection 
                             // of cars >20Km/h is always possible.

const int L_LANE_LED=10; //Lane 2 winning LED in D10 PIN
const int R_LANE_LED=11; //lane 1 winning LED on D11 PIN
const unsigned int MaxWaitTime=10000; //This is the maximum time to wait for second car to finish before recording result (millisecs)
const int buzzer=5; //Digital PIN buzzer is attached to - has to allow PWM (pin 3,5,6 or 9)

const int LEFT=1, RIGHT=2, BOTH=11;
const int RFDataIn = A2, RRDataIn = A3, LFDataIn = A0, LRDataIn = A1;  //Define the Anologue pins set up to which sensor. 
                                                                       // Reading Pin A3 causes Tone() to terminate early 
        //Beware NANO uses the LCD serial transmit (SDA,SCL) on pins A4 and A5
const int IRLEDThreshold = 800;   //Set to a value that indicates when the IR receiver is blocked. IR sensor returns 
                                  // 1024 at maximum brightness. 
                                  // Adjust for your configuration
const int DISTANCE_BETWEEN_SENSORS = 8; //set this to the distance (cm) between the front and rear IR sensor
const int L_LANE = 1, R_LANE = 2;
const int HIGH_TONE = 2400, LOW_TONE = 800;
const float SENSOR_DISTANCE_FACTOR  = float(288); // to speed up program, manually calculate this factor and place here. 
                                                  //  =float(DISTANCE_BETWEEN_SENSORS/100000)/float(0.27777778);
                                                  // SENSOR_DISTANCE_FACTOR is 288 if the DISTANCE_BETWEEN_SENSORS is 8cm

int FrontSensor[2], RearSensor[2]; //For recording the sensor information and allowing all subroutines to be able to read them. 

// Although array[0] should be available, my analysis shows that using array[0] is unreliable when using global variables.

void setup() {

  Serial.begin(9600);
  pinMode(RFDataIn, INPUT);
  pinMode(RRDataIn, INPUT);
  pinMode(LFDataIn, INPUT);
  pinMode(LRDataIn, INPUT);

  pinMode(R_LANE_LED, OUTPUT); //lane 1
  pinMode(L_LANE_LED, OUTPUT); //lane 2

  pinMode(buzzer, OUTPUT); // Set buzzer - as an output
  
  digitalWrite(R_LANE_LED, HIGH);
  digitalWrite(L_LANE_LED, HIGH);

  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Race timer");
  lcd.setCursor(2,1);
  lcd.print("Ian Cook");

  
}

void loop() {
 int Winner;
 bool R_LANEFinish, L_LANEFinish, R_LANESpeedTrap, L_LANESpeedTrap, Sound;
 float LSpeed, LScaleSpeed=0, RSpeed, RScaleSpeed=0, WinningDiff, RaceTime;
 int valRF, valLF, valRR, valLR;
 unsigned long StartTime, WaitTime, SampleTime, timediff;
 unsigned long FrontSensorTime[2],  RearSensorTime[2]; //Using array[0] results in a bug with global variables in the compilier, so I haven't used array[0]
 

  // Race reset - so reset variable values
  FrontSensorTime[L_LANE]= 0;
  FrontSensorTime[R_LANE]= 0;
  RearSensorTime[L_LANE] = 0;
  RearSensorTime[R_LANE] = 0;
  Winner = 99;
  LSpeed = 0;
  RSpeed = 0;
  WinningDiff = 999.999;
  RaceTime = 0;
  timediff =9999;
  R_LANEFinish = false;
  L_LANEFinish = false;
  R_LANESpeedTrap = false;
  L_LANESpeedTrap = false;

  //Flash lights,sound buzzer and declare we are ready to race
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Race Timer ");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("Ready to race...");
  digitalWrite(R_LANE_LED, LOW);
  digitalWrite(L_LANE_LED, LOW);
  tone(buzzer, LOW_TONE, 600); 
  delay(900); 
  tone(buzzer, LOW_TONE,600);
  digitalWrite(L_LANE_LED, HIGH); 
  delay(900);
  tone(buzzer, LOW_TONE,600);
  digitalWrite(R_LANE_LED,HIGH);
  delay(900);
  tone(buzzer, HIGH_TONE,800);
  digitalWrite(R_LANE_LED, LOW);
  digitalWrite(L_LANE_LED, LOW);
  Sound = true; //flag to allow tone to sound as workaround for Arduino bug that stops tone when pin A3 is read! 
  lcd.clear();
  lcd.setCursor(4,0); 
  lcd.print("GO GO GO!");
  StartTime = millis();  //set the start time of the race which begins when lights go out

  WaitTime = StartTime;  //Set this so that we don't wait for ever for a finishing car that has crashed

// Loop to wait until the first car crosses - the do Loop takes 1-2 ms which means speeds over 
// 144 Km/h will sometimes not be read. 288km/h is the max theoretical speed detectable on Arduino Nano
  do {  
//Read the sensor data and record the time
  SampleTime = SampleSensors(Sound);
    if (Sound && SampleTime>(StartTime+800)) Sound = false;  //Workaround for Tone bug that stops tone when pin A3 read!
    
//Set DEBUG flag for your setup to make sure the constant IRLEDThreshold value is accurate for your hardware
//If DEBUG is left true then these serial prints will slow the sample rate considerably and reduce the max detectable speed to approx 7Km/h
 #if (DEBUG) 
//   TestSensors(false);  //Debug routine to display the sensor recorded state
 #endif

// Loop for debugging the sensors. SHows how much flux on each one.

if (DEBUG_SENSORS) {
  while (true) {
    valRF = analogRead(RFDataIn);
    valLF = analogRead(LFDataIn);
    valRR = analogRead(RRDataIn);
    valLR = analogRead(LRDataIn);

    Serial.print ("Left: Front: ");
    Serial.print(valLF);
    Serial.print("\t Rear: ");
    Serial.print(valLR);

    Serial.print("Right: Front:");
    Serial.print(valRF);
    Serial.print("\t Rear");
    Serial.println(valRR);
  }
 }
           
    if (!L_LANEFinish && (FrontSensor[L_LANE] < IRLEDThreshold)) {
      // Left Lane has finished
      FrontSensorTime[L_LANE] = SampleTime;
      if (!R_LANEFinish) {
        //As the other lane hasn't finished yet, this lane is the winner
        Winner = L_LANE;
        digitalWrite(L_LANE_LED, HIGH);
       }
      L_LANEFinish = true;
      WaitTime = millis();  //Reset the timer to ensure we don't wait if the other lane has crashed
     }
    if (!R_LANEFinish && (FrontSensor[R_LANE] < IRLEDThreshold)) {
      // Right Lane has finished
      FrontSensorTime[R_LANE] = SampleTime;
      if (!L_LANEFinish) {
        //As the other lane hasn't finished yet, this lane is the winner
        Winner = R_LANE;
        digitalWrite(R_LANE_LED, HIGH);
       }
      R_LANEFinish = true;
      WaitTime = millis(); //Reset the timer to ensure we don't wait if the other lane has crashed
     }
      
    if (L_LANEFinish && !L_LANESpeedTrap && RearSensor[L_LANE] < IRLEDThreshold) {
      //The car has crossed the line and now the rear sensor
      RearSensorTime[L_LANE] = SampleTime;
      L_LANESpeedTrap = true;
    }
    if (R_LANEFinish && !R_LANESpeedTrap && RearSensor[R_LANE] < IRLEDThreshold) {
      //The car has crossed the line and now the rear sensor
      RearSensorTime[R_LANE] = SampleTime;
      R_LANESpeedTrap = true;
    }
// Exit the loop once all sensors have been triggered or once one lane has finished and there has been a delay > MaxWaitTime   
  } while (!((L_LANEFinish && R_LANEFinish && L_LANESpeedTrap && R_LANESpeedTrap) || ((L_LANEFinish || R_LANEFinish) && (millis() - WaitTime > MaxWaitTime)) ) );

  tone(buzzer, HIGH_TONE,150);
  delay(200);
  tone(buzzer, HIGH_TONE,150);
    
// Send information to a connected computer which can be used in data analysis
#if (COMP_OUTPUT)
    Serial.println("Times:\tLEFT Front,\tRear,\tRIGHT Front,\tRear");
    Serial.print("\t");
    Serial.print(FrontSensorTime[L_LANE]);
    Serial.print("\t");
    Serial.print(RearSensorTime[L_LANE]);
    Serial.print("\t\t");
    Serial.print(FrontSensorTime[R_LANE]);
    Serial.print("\t");
    Serial.println(RearSensorTime[R_LANE]);
#endif
//Race is over.  Calculate and display times
//Race Time is time for first car to finish

switch (Winner) {
  case R_LANE:
    RaceTime = float (FrontSensorTime[R_LANE]-StartTime)/1000; //Recorded in Sec
    if (FrontSensorTime[L_LANE]>0) WinningDiff = float(FrontSensorTime[L_LANE] - FrontSensorTime[R_LANE])/1000;   
   break;
  case L_LANE:
   RaceTime = float (FrontSensorTime[L_LANE]-StartTime)/1000;
   if (FrontSensorTime[R_LANE]>0) WinningDiff = float(FrontSensorTime[R_LANE] - FrontSensorTime[L_LANE])/1000;
   break;
  default:
   RaceTime = 0;
}

 //Check for a draw due to sample timings being equal
 if (WinningDiff == 0) Winner=555;
   
//The distance between the sensors is DistanceBetweenSensors (cm) so the speed is D cm/time taken (cm/ms *3 600 000 / 100 000 = km/h) 
// :: Millis() returns milliseconds which 1/1000 s
// Km/h = (D/100000)/(milli/(1000*60*60)) = D/(milli/3600) or D/3600 * 1/milli; D=8 then km/h = 288 / milli
//Max speed capable of reading at 2ms sample rate is  144 Km/h!
// Speed = Dist in KM (cm /100000)  / time in hrs ( millis/1000/60/60) 
// distance = 8 cm then Speed = (8/100000)/(millis/.27777778) = (0.00008/0.27777778)/millis = 288/millis

  timediff = RearSensorTime[L_LANE] - FrontSensorTime[L_LANE];
  if (timediff>0) {
   LSpeed = SENSOR_DISTANCE_FACTOR / float(timediff); //km/h
   #if (DEBUG) 
    //Use to check that the SENSOR_DISTANCE_FACTOR is correct for your set up
    Serial.print ("Time Diff:\t");
    Serial.print (timediff);
    Serial.print (" Lspeed calc ");
    Serial.println (LSpeed);
    #endif
  }
  else LSpeed = 0;

  timediff = RearSensorTime[R_LANE] - FrontSensorTime[R_LANE];
  if (timediff>0) {
    RSpeed = SENSOR_DISTANCE_FACTOR / float(timediff); //km/h
    #if (DEBUG)
      //Use to check that the SENSOR_DISTANCE_FACTOR is correct for your set up
      Serial.print ("Time Diff:\t");
      Serial.print (timediff);
      Serial.print (" Rspeed calc ");
      Serial.println (RSpeed);
    #endif
  }
  else RSpeed = 0;

//Hot wheels cars are 1:64 scale so calculate scale speed
  LScaleSpeed=LSpeed*float(64);
  RScaleSpeed=RSpeed*float(64);

//Send information to serial device (computer) for data analysis
 #if (COMP_OUTPUT) 
  Serial.print("Race Time: ");
  Serial.println(RaceTime);
  Serial.println("Final Speeds");
  Serial.print("LEFT:  ");
  Serial.print(LSpeed,3);
  Serial.print(" Km/h : Scale Spd= ");
  Serial.print(LScaleSpeed,2);
  Serial.println(" Km/h");
  Serial.print("RIGHT: ");
  Serial.print(RSpeed,3);
  Serial.print(" Km/h : Scale Spd= ");
  Serial.print(RScaleSpeed,2);
  Serial.println(" Km/h");

  Serial.print("Left  Lane Race Time (ms)");
  Serial.println("\tRight Lane Race Time (ms)");
  Serial.print(FrontSensorTime[L_LANE]);
  Serial.print("\t\t\t");
  Serial.println(FrontSensorTime[R_LANE]);
  if (Winner== 555) Serial.print("This was a DRAW! ");
    else if (Winner == R_LANE) Serial.print("Right car won by ");
         else Serial.print("Left car won by ");
  Serial.print(WinningDiff,3);
  Serial.println(" s");
 #endif
 
//Send the information to the LCD display on the hardware unit
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Race Time:");
  lcd.setCursor(10,0);
  lcd.print(RaceTime,3);
  lcd.setCursor(14,0);
  lcd.print("s ");
  lcd.setCursor(0,1);
  if (Winner == L_LANE) {
    lcd.print("L won by");
    }
  else if (Winner == R_LANE) {
    lcd.print("R won by");
    }
    else {
      lcd.print("DRAW!");
   }
  lcd.setCursor(10,1);
  lcd.print(WinningDiff,3);
  lcd.setCursor(15,1);
  lcd.print("s");
  delay(5000);
    
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LSpd:");
  lcd.setCursor(5, 0);
  if (LSpeed > 0.001) {
    lcd.print(LSpeed, 2);  
    lcd.setCursor(11, 0);
    lcd.print(" Km/h");
   }
   else lcd.print("  * DNF *");  //This lane did not finish
  lcd.setCursor(0, 1);
  lcd.print("RSpd:" );
  lcd.setCursor(5, 1);
  if (RSpeed > 0.001) {
    lcd.print(RSpeed, 2);  
    lcd.setCursor(11, 1);
    lcd.print(" Km/h");
   }
   else lcd.print("  * DNF *");  //This lane did not finish

  if (Winner != 555) {
    //i.e. This is NOT a draw
    //set cursor for Winnner text
    delay(1000);
    lcd.setCursor(11, (Winner-L_LANE));
    lcd.print("*WIN*");
    for (int i=0; i<6; i++) {
      digitalWrite(Winner+9, LOW);
      delay(400);
      digitalWrite(Winner+9, HIGH);
      delay(400);
      }
   }
   else {
   //This must be a draw
    lcd.setCursor(12,0);
    lcd.print ("DRAW");
    lcd.setCursor(12,1);
    lcd.print ("DRAW");
    for (int i=0; i<6; i++) {
     digitalWrite(L_LANE, LOW);
     digitalWrite(R_LANE, LOW);
     delay(400);
     digitalWrite(R_LANE, HIGH);
     digitalWrite(L_LANE, HIGH);
     delay(400);
     }   
    }

   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("LScaSpd:");
   if (LScaleSpeed<10)   lcd.setCursor(12,0);
    else {
      if (LScaleSpeed<100)  lcd.setCursor(11, 0); //line up the decimal points on the display
      else lcd.setCursor(10,0);
    }
   if (LScaleSpeed >0) lcd.print(LScaleSpeed, 1);
     else lcd.print("DNF");
   lcd.setCursor(0, 1);
   lcd.print("RScaSpd:");
   if (RScaleSpeed<10)   lcd.setCursor(12,1);
    else {
      if (RScaleSpeed<100)  lcd.setCursor(11, 1); //line up the decimal points on the display
      else lcd.setCursor(10,1);
    }
   if (RScaleSpeed >0) lcd.print(RScaleSpeed, 1);
     else lcd.print("DNF");

  // Delay before staring the race again.
  delay(5000);
}


unsigned long SampleSensors(bool sound) {
//Read the sensor data and return the time they are taken (ms)
//The parameter sound is used to prevent the tone being terminated early due to Arduino Nano issue that reading A3 pin causes the tone to stop!
  FrontSensor[R_LANE] = analogRead(RFDataIn);
  FrontSensor[L_LANE] = analogRead(LFDataIn);
//This section only compiles the relevant code if RRDataIn is connected to pin A3, defined in constants. This is to prevent cutting tone short during start sequence
#if RRDataIn == A3
  if (!sound) RearSensor[R_LANE] = analogRead(RRDataIn);
#else
    RearSensor[R_LANE] = analogRead(RRDataIn); 
#endif
  RearSensor[L_LANE] = analogRead(LRDataIn);
  return(millis());
}
