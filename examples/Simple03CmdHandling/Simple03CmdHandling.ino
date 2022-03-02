#include "ModBusSlave0.h"

#define TRANSMIT_PIN 6
#define MODBUS_ADDRESS 100

ModBusSlave0 mod;

ISR(USART0_RX_vect)
{
  mod.receiveHandler();
}

ISR(USART0_TX_vect)
{
  mod.transmitHandler();
}

uint8_t prepareResponse03(uint8_t *frame, uint8_t* data) {
  if (frame[3] == 1) {
    data[2] = 3;
    data[5] = 4;
  }
  return 6;
}

void setup() {
  mod.begin(57600, TRANSMIT_PIN, MODBUS_ADDRESS);
  mod.prepareResponse03 = *prepareResponse03;
}

void loop() {
  mod.process();
}
