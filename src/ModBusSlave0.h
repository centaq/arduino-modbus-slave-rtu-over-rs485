#ifndef ModBusSlave0_h
#define ModBusSlave0_h

#include <Arduino.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "ModBusSlave.h"



class ModBusSlave0: public ModBusSlave
{
  public:
    ModBusSlave0(bool crcBigEndian) : ModBusSlave(&UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0, crcBigEndian) { };
    ModBusSlave0() : ModBusSlave(&UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0, true) { };
};


#endif
