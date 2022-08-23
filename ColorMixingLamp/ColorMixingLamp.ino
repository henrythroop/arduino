/*
  Arduino Starter Kit example
  Project 4 - Color Mixing Lamp

  This sketch is written to accompany Project 3 in the Arduino Starter Kit

  Parts required:
  - one RGB LED
  - three 10 kilohm resistors
  - three 220 ohm resistors
  - three photoresistors
  - red green and blue colored gels

  created 13 Sep 2012
  modified 14 Nov 2012
  by Scott Fitzgerald
  Thanks to Federico Vanzati for improvements

  https://store.arduino.cc/genuino-starter-kit

  This example code is part of the public domain.
*/

const int greenLEDPin = 9;    // LED connected to digital pin 9
const int redLEDPin = 10;     // LED connected to digital pin 10
const int blueLEDPin = 11;    // LED connected to digital pin 11

const int redSensorPin = A0;  // pin with the photoresistor with the red gel
const int greenSensorPin = A1;   // pin with the photoresistor with the green gel
const int blueSensorPin = A2;   // pin with the photoresistor with the blue gel

int redValue = 0; // value to write to the red LED
int greenValue = 0; // value to write to the green LED
int blueValue = 0; // value to write to the blue LED

int redSensorValue = 0; // variable to hold the value from the red sensor
int greenSensorValue = 0; // variable to hold the value from the green sensor
int blueSensorValue = 0; // variable to hold the value from the blue sensor

int offsetRed = 0;
int offsetGreen = 0;
int offsetBlue = 0;

int scale = 1;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  // set the digital pins as outputs
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
}

void loop() {
  // Read the sensors first:

  // read the value from the red-filtered photoresistor:
  redSensorValue = analogRead(redSensorPin);
  // give the ADC a moment to settle
  delay(5);
  // read the value from the green-filtered photoresistor:
  greenSensorValue = analogRead(greenSensorPin);
  // give the ADC a moment to settle
  delay(5);
  // read the value from the blue-filtered photoresistor:
  blueSensorValue = analogRead(blueSensorPin);

  // print out the values to the Serial Monitor
//  Serial.print("raw sensor Values \t red: ");
//  Serial.print(redSensorValue);
//  Serial.print("\t green: ");
//  Serial.print(greenSensorValue);
//  Serial.print("\t Blue: ");
//  Serial.println(blueSensorValue);

  /*
    In order to use the values from the sensor for the LED, you need to do some
    math. The ADC provides a 10-bit number, but analogWrite() uses 8 bits.
    You'll want to divide your sensor readings by 4 to keep them in range
    of the output.
  */

  offsetGreen = 450; // Value at zero flux level
  offsetRed   = 450;
  offsetBlue  = 450;
  
  scale = 3;
  
  redValue = (redSensorValue-offsetRed) / scale;
  greenValue = (greenSensorValue-offsetGreen) / scale;
  blueValue = (blueSensorValue-offsetBlue) / scale;

  if (redValue < 0)
    { redValue = 0;}

if (greenValue < 0)
    { greenValue = 0;}

if (blueValue < 0)
    { blueValue = 0;}

//    // print out the mapped values
//    Serial.print("Mapped sensor Values \t red: ");
//    Serial.print(redValue);
//    Serial.print("\t green: ");
//    Serial.print(greenValue);
//    Serial.print("\t Blue: ");
//    Serial.println(blueValue);

    Serial.print ("Red:");
    Serial.print(redSensorValue);
    Serial.print("\t");
    Serial.print(redValue);

    Serial.print("\t");
    Serial.print ("Green:");
    Serial.print(greenSensorValue);
    Serial.print("\t");
    Serial.print(greenValue);

    Serial.print("\t");
    Serial.print ("Blue:");
    Serial.print(blueSensorValue);
    Serial.print("\t");
    Serial.println(blueValue);
  
  /*
    Now that you have a usable value, it's time to PWM the LED.
  */
  analogWrite(redLEDPin, redValue);
  analogWrite(greenLEDPin, greenValue);
  analogWrite(blueLEDPin, blueValue);

  // Now play a tone on the speakers

  int pitch = map(blueValue, 0, 128, 50, 4000);
  tone(6, pitch, 10);
  delay (20);

  pitch = map(greenValue, 0, 128, 100, 8000);
  tone(8, pitch, 10);
  delay (20); 

  pitch = map(redValue, 0, 128, 150, 12000);
  tone(3, pitch, 10);
  delay(20);
  
  
}
