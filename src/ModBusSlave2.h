#ifndef ModBusSlave2_h
#define ModBusSlave2_h

#include <Arduino.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "ModBusSlave.h"



class ModBusSlave2: public ModBusSlave
{
  public:
    ModBusSlave2(bool crcBigEndian) : ModBusSlave(&UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UCSR2C, &UDR2, crcBigEndian) { };
    ModBusSlave2() : ModBusSlave(&UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UCSR2C, &UDR2, true) { };
};


#endif
