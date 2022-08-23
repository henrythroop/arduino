/*
  fastIO.h
  A simple and fast (2.5x) pin control.
  Управление выводами (быстрее ~2,5 раза).
  Created by RIVA, 2019.
*/


#include "fastIO.h"


const uint8_t mask[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};


void fastPinMode(uint8_t pin, uint8_t mode) {

  if        (pin < 8 ) {
    if (mode & 0x01) {
      DDRD  |= mask[pin];
    } else {
      (mode & 0x02) ? PORTD |= mask[pin] : PORTD &= ~mask[pin];
      DDRD  &= ~mask[pin];
    }
    //    // v1.0
    //    (mode & 0x01) ? DDRD  |= mask[pin] : DDRD  &= ~mask[pin];
    //    (mode & 0x02) ? PORTD |= mask[pin] : PORTD &= ~mask[pin];


  } else if (pin < 14) {

    pin -= 8;
    if (mode & 0x01) {
      DDRB  |= mask[pin];
    } else {
      (mode & 0x02) ? PORTB |= mask[pin] : PORTB &= ~mask[pin];
      DDRB  &= ~mask[pin];
    }
    //    // v1.0
    //    (mode & 0x01) ? DDRB  |= mask[pin] : DDRB  &= ~mask[pin];
    //    (mode & 0x02) ? PORTB |= mask[pin] : PORTB &= ~mask[pin];


  } else if (pin < 22) {

    pin -= 14;
    if (mode & 0x01) {
      DDRC  |= mask[pin];
    } else {
      (mode & 0x02) ? PORTC |= mask[pin] : PORTC &= ~mask[pin];
      DDRC  &= ~mask[pin];
    }
    //    // v1.0
    //    (mode & 0x01) ? DDRC  |= mask[pin] : DDRC  &= ~mask[pin];
    //    (mode & 0x02) ? PORTC |= mask[pin] : PORTC &= ~mask[pin];

  }

}


void fastDigitalWrite(uint8_t pin, uint8_t state) {

  if        (pin < 8 ) {

    state ? PORTD |= mask[pin] : PORTD &= ~mask[pin];

  } else if (pin < 14) {

    pin -= 8;
    state ? PORTB |= mask[pin] : PORTB &= ~mask[pin];

  } else if (pin < 22) {

    pin -= 14;
    state ? PORTC |= mask[pin] : PORTC &= ~mask[pin];
  }

}


uint8_t fastDigitalRead(uint8_t pin) {

  if        (pin < 8 ) {

    return (PIND & mask[pin]) ? 1 : 0;

  } else if (pin < 14) {

    pin -= 8;
    return (PINB & mask[pin]) ? 1 : 0;

  } else if (pin < 22) {

    pin -= 14;
    return (PINC & mask[pin]) ? 1 : 0;
  }

}


void fastShiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t value) {
  uint8_t clk = fastDigitalRead(clockPin);

  for (uint8_t i = 0; i < 8; i++) {

    if (bitOrder == LSBFIRST) {
      fastDigitalWrite(dataPin, value & 1);
      value >>= 1;
    }
    else {
      fastDigitalWrite(dataPin, (value & 0x80) > 0);
      value <<= 1;
    }

    fastDigitalWrite(clockPin, !clk);
    fastDigitalWrite(clockPin,  clk);
  }

}


uint8_t fastShiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
  uint8_t value = 0;
  uint8_t clk   = fastDigitalRead(clockPin);

  for (uint8_t i = 0; i < 8; i++) {

    fastDigitalWrite(clockPin, !clk);

    if (bitOrder == LSBFIRST) {
      value  |= fastDigitalRead(dataPin) << 7;
      value >>= 1;
    }
    else {
      value  |= fastDigitalRead(dataPin);
      value <<= 1;
    }

    fastDigitalWrite(clockPin, clk);
  }

  return value;
}

