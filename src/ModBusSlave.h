#ifndef ModBusSlave_h
#define ModBusSlave_h

#include <Arduino.h>
#include <util/delay.h>
#include <util/atomic.h>

#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 128

//#define MODBUS_SLAVE_DEBUG

struct response_data
{
	uint8_t len;
	uint8_t cmd;
	uint8_t* data;
};

class ModBusSlave
{
  public:
    ModBusSlave(
      volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
      volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
      volatile uint8_t *ucsrc, volatile uint8_t *udr, bool crcBigEndian);
	
	void begin(uint32_t baud, uint8_t TE_pin, uint8_t mb_addr);
	
	void process(); 
		
	void receiveHandler();
	void transmitHandler();
	void test();
	
    uint8_t (*prepareResponse01)(uint8_t *frame, char *data);
    uint8_t (*prepareResponse03)(uint8_t *frame, char *data);
    uint8_t (*prepareResponse06)(uint8_t *frame, char *data);
    uint8_t (*prepareResponse16)(uint8_t *frame, char *data);
    uint8_t prepareResponseXX(uint8_t functionCode, uint8_t *frame, char *data);
	void (*afterFinishMethod)();
	
  protected:
    volatile uint8_t * const _ubrrh;
    volatile uint8_t * const _ubrrl;
    volatile uint8_t * const _ucsra;
    volatile uint8_t * const _ucsrb;
    volatile uint8_t * const _ucsrc;
    volatile uint8_t * const _udr;
	
  private:
	bool _crcBigEndian;
	
	uint8_t _TE_pin;
	uint8_t _mb_addr;
	
	bool _receiving;
	bool _transmitting;
	uint32_t _receivingCurrentTime;
	uint32_t _receivingStartTime;
	uint32_t _t35break;
	
	uint8_t _rx_buffer[RX_BUFFER_SIZE];
	uint8_t _rx_index;
	
	void startUSART(uint32_t baud);
	void resetUSARTTransmitFlag();
	unsigned char waitAndGetReceivedChar();
	void transmitSingle(unsigned char data);
	
	void transmitMode();
	void receiveMode();
	
	void prepareResponse();
	
	bool isTxAvailable();
	char _tx_buffer[TX_BUFFER_SIZE];
	uint8_t _tx_index;
	uint8_t _tx_size;
	
	
	uint16_t generateCRC(uint8_t* data, uint8_t size);
	
};
#endif
