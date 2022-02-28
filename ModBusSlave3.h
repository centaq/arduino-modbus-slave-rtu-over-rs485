#ifndef ModBusSlave3_h
#define ModBusSlave3_h

#include <Arduino.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "ModBusSlave.h"



class ModBusSlave3: public ModBusSlave
{
  public:
    ModBusSlave3(bool crcBigEndian) : ModBusSlave(&UBRR3H, &UBRR3L, &UCSR3A, &UCSR3B, &UCSR3C, &UDR3, crcBigEndian) { };
    ModBusSlave3() : ModBusSlave(&UBRR3H, &UBRR3L, &UCSR3A, &UCSR3B, &UCSR3C, &UDR3, true) { };
};


#endif
