/*
  fastIO.h
  A simple and fast (2.5x) pin control.
  Управление выводами (быстрее ~2,5 раза).
  Created by RIVA, 2019.

  2021.06.04 - v1.1
  - исправлена функция fastPinMode (некорректная уст. реж. выхода)
*/

#ifndef FAST_IO_H
#define FAST_IO_H


#if defined(ARDUINO) && (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#include <pins_arduino.h>
#endif


#define FAST_IO_IMPLEMENTED

#ifdef FAST_IO_IMPLEMENTED

void    fastPinMode     (uint8_t pin, uint8_t mode);
void    fastDigitalWrite(uint8_t pin, uint8_t state);
uint8_t fastDigitalRead (uint8_t pin);
void    fastShiftOut    (uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t value);
uint8_t fastShiftIn     (uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

#else

#define fastPinMode         pinMode
#define fastDigitalWrite    digitalWrite
#define fastDigitalRead     digitalRead
#define fastShiftOut        shiftOut
#define fastShiftIn         shiftIn

#endif

#endif /* FAST_IO_H */
