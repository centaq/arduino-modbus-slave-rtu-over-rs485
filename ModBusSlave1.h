#ifndef ModBusSlave1_h
#define ModBusSlave1_h

#include <Arduino.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "ModBusSlave.h"



class ModBusSlave1: public ModBusSlave
{
  public:
    ModBusSlave1(bool crcBigEndian) : ModBusSlave(&UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UCSR1C, &UDR1, crcBigEndian) { };
    ModBusSlave1() : ModBusSlave(&UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UCSR1C, &UDR1, true) { };
};


#endif
