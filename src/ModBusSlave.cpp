
#include "ModBusSlave.h"

ModBusSlave::ModBusSlave(
  volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
  volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
  volatile uint8_t *ucsrc, volatile uint8_t *udr, bool crcBigEndian) :
    _ubrrh(ubrrh), _ubrrl(ubrrl),
    _ucsra(ucsra), _ucsrb(ucsrb), 
	_ucsrc(ucsrc), _udr(udr),
	_crcBigEndian(crcBigEndian)
{
}

void ModBusSlave::begin(uint32_t baud, uint8_t TE_pin, uint8_t mb_addr) {
	startUSART(baud);
	
	_TE_pin = TE_pin;
	_mb_addr = mb_addr;
	
	_t35break = (1000000 / (baud / 10)) * 4; //waiting for approximately 4 chars

	pinMode(_TE_pin, OUTPUT);
	receiveMode();
}

void ModBusSlave::startUSART(uint32_t baud) {
	unsigned int ubrr = F_CPU/16/baud-1;
	*_ubrrh = (unsigned char)(ubrr>>8);
	*_ubrrl = (unsigned char)ubrr;
	*_ucsrb = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0);
	*_ucsrc |= (3<<UCSZ00);
}

void ModBusSlave::resetUSARTTransmitFlag() {
	*_ucsra &=~_BV(TXC0);
}

unsigned char ModBusSlave::waitAndGetReceivedChar() {	
	while (!(*_ucsra & (1<<RXC0)));
	return *_udr;
}

void ModBusSlave::transmitSingle(unsigned char data) {
	while (!(*_ucsra & (1<<UDRE0)));
	*_udr = data;
}

void ModBusSlave::process() {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_receivingCurrentTime = micros();
		uint32_t deltaTime = _receivingCurrentTime - _receivingStartTime;
  
		if (_receiving && deltaTime > _t35break) {
			_receiving = false;
			_receivingStartTime = 0;
			_tx_index = 0;
			_tx_size = 0;
			if (_rx_buffer[0] == _mb_addr) { //TODO add CRC checking
				prepareResponse();
			}
			
			_rx_index = 0;
		}
	}
	if (_tx_index == 0 &&  _tx_size > 0) {
		resetUSARTTransmitFlag();
		transmitHandler();
	}
}

void ModBusSlave::receiveHandler() {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_receiving = true;
		_receivingStartTime = micros();
		
		_rx_buffer[_rx_index++] = waitAndGetReceivedChar();
	}
}

void ModBusSlave::transmitHandler() {
	if (!_transmitting && isTxAvailable()) {
		_transmitting = true;
		transmitMode();
	}
	if (_transmitting && isTxAvailable()) {
		transmitSingle(_tx_buffer[_tx_index++]);
	} else if (_transmitting && _tx_index == _tx_size) {
		_transmitting = false;
		_tx_index = 0;
		_tx_size = 0;
	
		for (uint8_t i = 0; i < TX_BUFFER_SIZE - 1; i++) {
			_tx_buffer[i] = '\0';
		}
		_tx_buffer[TX_BUFFER_SIZE - 1] = '\0';
		
		receiveMode();
		if (afterFinishMethod)
			afterFinishMethod();
	}
}

void ModBusSlave::prepareResponse() {
	char data[128];
	
	uint8_t functionCode = _rx_buffer[1];

	uint8_t len = prepareResponseXX(functionCode, _rx_buffer, data);
  
	_tx_size = 0;
	
	_tx_buffer[_tx_size++] = _mb_addr;
	_tx_buffer[_tx_size++] = functionCode;
	
	if (len > 0) {		
		if (functionCode >= 1 && functionCode <= 4)
			_tx_buffer[_tx_size++] = len;

		for (uint8_t i = 0; i < len; i++) {
			_tx_buffer[_tx_size++] = (data[i]);
		}
	} else {
		_tx_buffer[_tx_size++] = 0x80 | functionCode;
		_tx_buffer[_tx_size++] = 1;
	}
	
	uint16_t crc = generateCRC(_tx_buffer, _tx_size);
	
	if (_crcBigEndian) {
		_tx_buffer[_tx_size++] = crc >> 8;
		_tx_buffer[_tx_size++] = crc;
	} else {
		_tx_buffer[_tx_size++] = crc;
		_tx_buffer[_tx_size++] = crc >> 8;
	}
}

uint8_t ModBusSlave::prepareResponseXX(uint8_t functionCode, uint8_t *frame, char *data) {
  uint8_t len = 0;
  switch (functionCode) {
    case 1:
      if (prepareResponse01) {
        len = prepareResponse01(frame, data);
      }
      break;
    case 3:
      if (prepareResponse03) {
        len = prepareResponse03(frame, data);
      }
      break;
    case 6:
      if (prepareResponse06) {
        len = prepareResponse06(frame, data);
      }
      break;
    case 16:
      if (prepareResponse16) {
        len = prepareResponse16(frame, data);
      }
      break;
    default:
      break;
  }
  
  return len;
}

bool ModBusSlave::isTxAvailable() {
	return _tx_index < _tx_size;
}

uint16_t ModBusSlave::generateCRC(uint8_t* data, uint8_t size)
{
	uint16_t crc = 0xFFFF;
	for (uint8_t i = 0; i < size; i++)
	{
		crc ^= (uint16_t)data[i];
		for (uint8_t j = 0; j < 8; j++)
		{
			if ((crc & 0x0001) != 0)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
				crc >>= 1;
		}
	}
	return crc;
}


void ModBusSlave::receiveMode() {
	digitalWrite(_TE_pin, LOW);
}

void ModBusSlave::transmitMode() {
	digitalWrite(_TE_pin, HIGH);
	delay(5);
}
