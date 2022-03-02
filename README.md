
# arduino-modbus-slave-rtu-over-rs485
Arduino ModBus RTU Slave library using RS485

This library was created to make it easier to create communication between devices using ModBus RTU protocol, based on RS485 wiring.

As it state in the title, library is written only for handling slave part of communication. And currently it is handling only couple of commands.

### History note
First version of this library (not published) was made using HardwareSerial from Arduino IDE. Unfortunatelly this result with message round trip times being around 900 ms, which was unacceptable (I might be doing something wrong ;) ).
Right now they are on expected level, around 30ms depending of course on message length.

## Requirements
### Software level
None of which I'm aware of.
### Hardware level
Communication over RS485, which mainly means that there is a output pin to determine if RS485 module is transmitting or receiving data.

## Examples
### Simple `03` command handler 

    include "ModBusSlave0.h"
    
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
As you can see you have to do some configuration work on your own. Which is setting up USART event vectors for sending and receiving and declare BAUD_RATE, TRANSMIT_PIN and device MODBUS_ADDRESS.

Transmit pin is set to high level when the device starts transmitting.

You also have to set function handler for prepareResponse for cmd `03`
Each of these prepareResponseXX functions requires the length of the data part of response to be returned.

Additionally you can use overloaded constructor passing information about CRC part of the reponse indicating if it should be send as BigEndian - (true) - default. 
It was prepared as some devices expecting it to be other way around, e.g Siemens PLCs.

### Full cmd handling

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
Its pretty much the same as previous example with handler for `06` and `16` commands.
As you can see, the response length and content for those are strictly related to the request.


## Limitations
1. You cannot simultaneously use this library and HardwareSerial from Arduino IDE for the same USART. 
2. Single ModBusSlave class requires around 300 bytes of memory allocated
3. Code developed and tested only on Arduino MEGA 2560. Looking on HardwareSerial implementation, they for sure will be some problems with Atmega8 based modules. Maybe others
4. Library only support 4 command types `01`, `03`, `06`, `16`
5. Library supports only messages shorter than 128 bytes long. Both incoming and outgoing. And both can be changed in header file.