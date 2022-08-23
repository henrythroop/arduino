#include "fastIO.h"

#define SAMPLES 1024

/*******************************************************************************/

uint32_t timestamp;

void timeMeasureStart() {
  timestamp = micros();
}

void timeMeasureStop() {
  timestamp = micros() - timestamp;

  Serial.print(F(" [wait time = "));
  Serial.print(timestamp / (float)(SAMPLES), 3);
  Serial.print(F(" us]"));
}

uint32_t timeMeasureValue() {
  return timestamp;
}

/*******************************************************************************/

void consoleTest() {
  Serial.print(F("fastPinMode(13, INPUT)"));
  timeMeasureStart();
  for (uint16_t i = 0; i < SAMPLES; i++) {
    fastPinMode(13, INPUT);
  }
  timeMeasureStop();
  Serial.println();

  Serial.print(F("fastPinMode(13, INPUT_PULLUP)"));
  timeMeasureStart();
  for (uint16_t i = 0; i < SAMPLES; i++) {
    fastPinMode(13, INPUT_PULLUP);
  }
  timeMeasureStop();
  Serial.println();

  Serial.print(F("fastPinMode(13, OUTPUT)"));
  timeMeasureStart();
  for (uint16_t i = 0; i < SAMPLES; i++) {
    fastPinMode(13, OUTPUT);
  }
  timeMeasureStop();
  Serial.println();


  Serial.print(F("pinMode(13, INPUT)"));
  timeMeasureStart();
  for (uint16_t i = 0; i < SAMPLES; i++) pinMode(13, INPUT);
  timeMeasureStop();
  Serial.println();

  Serial.print(F("pinMode(13, INPUT_PULLUP)"));
  timeMeasureStart();
  for (uint16_t i = 0; i < SAMPLES; i++) pinMode(13, INPUT_PULLUP);
  timeMeasureStop();
  Serial.println();

  Serial.print(F("pinMode(13, OUTPUT)"));
  timeMeasureStart();
  for (uint16_t i = 0; i < SAMPLES; i++) pinMode(13, OUTPUT);
  timeMeasureStop();
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  fastPinMode(A1, OUTPUT);
  fastDigitalWrite(A1, 1);

  consoleTest();
}


void loop() {
}
