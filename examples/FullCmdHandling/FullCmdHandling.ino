#include "ModBusSlave0.h"

#define TRANSMIT_PIN 6
#define MODBUS_ADDRESS 100

ModBusSlave0 modbus;

ISR(USART0_RX_vect)
{
  modbus.receiveHandler();
}

ISR(USART0_TX_vect)
{
  modbus.transmitHandler();
}

uint8_t prepareResponse03(uint8_t *frame, uint8_t* data) {
  if (frame[3] == 1) {
    data[2] = 3;
    data[5] = 4;
  }
  return 6;
}

uint8_t prepareResponse06(uint8_t *frame, uint8_t* data) {
  if (frame[2] == 1) {
    uint8_t smsType = frame[3];
    uint8_t recipient = frame[4];
    uint8_t receivedData[1];
    receivedData[0] = frame[5];
    //Do something with request
  }
  
  for(uint8_t i = 0; i < 4; i++) 
    data[i] = frame[i+2];
  return 4;
}

uint8_t prepareResponse16(uint8_t *frame, uint8_t* data) {
  if (frame[2] == 1) {
    uint8_t smsType = frame[3];
    uint8_t recipient = frame[7];
    uint8_t dataLen = frame[6];
    uint8_t receivedData[8];
    for (uint8_t i = 0; i < dataLen - 1 && i < 8; i++) {
      receivedData[i] = frame[9 + i];
    }
    //Do something with data
  }
  
  for(uint8_t i = 0; i < 4; i++) 
    data[i] = frame[i+2];
  return 4;
}

void setup() {
  modbus.begin(57600, TRANSMIT_PIN, MODBUS_ADDRESS);
  modbus.prepareResponse03 = *prepareResponse03;
  modbus.prepareResponse06 = *prepareResponse06;
  modbus.prepareResponse16 = *prepareResponse16;
}

void loop() {
  modbus.process();
}
