#include "SimpleModbusSlave.h"

#define BUFFER_SIZE 128

// frame[] is used to receive and transmit packages.
uint8_t frame[BUFFER_SIZE];
uint8_t slaveID;
uint16_t holdingRegsSize; // size of the register array
uint8_t broadcastFlag;
uint8_t function;
uint8_t TxEnablePin;
uint16_t errorCount;
uint16_t T1_5; // inter character time out
uint16_t T3_5; // frame delay
uint16_t counter;

// function definitions
void exceptionResponse(uint8_t exception);
uint16_t calculateCRC(uint8_t bufferSize);
void sendPacket(uint8_t bufferSize);
void UART0_Init(uint32_t baudrate);

uint16_t modbus_update(uint16_t *holdingRegs){
	// variables for receive from uart
  uint8_t bufferIdx = 0;
  uint8_t overflow = 0;
	
	// Variables for Check MB frame by CRC
	uint16_t rcv_crc;
	uint8_t rcv_id;
	
	//Variables for Decode data fields	
	uint16_t startingAddress, no_of_registers, maxData;
	
	//Variables for  prepare MB frame response (payload)
	uint8_t index;
  uint8_t address;
  uint16_t crc16;
	uint16_t regStatus;
	uint8_t noOfBytes, responseFrameSize;
	uint16_t temp;
	
	//  receive bytes from UART built-in bufffer (M054 has 16 bytes)
	while (!UART_GET_RX_EMPTY(UART0)){
		if (overflow)
			UART_READ(UART0);
		else{
			if (bufferIdx == BUFFER_SIZE)
				overflow = 1;	
			
			//frame[bufferIdx] = UART_READ(UART0);
			UART_Read(UART0,frame+bufferIdx,1);
			bufferIdx++;
		}
		CLK_SysTickDelay(T1_5); // delay T1_5 btw characters
	}
	// If an overflow occurred increment the errorCount
	// variable and return to the main sketch without
	// responding to the request i.e. force a timeout
	if (overflow)
		return errorCount++;
	
	// The minimum request packet is 8 bytes for function 3 & 16
	if (bufferIdx > 6) {
		rcv_id = frame[0];
    broadcastFlag = 0;
		// Check Broadcast ID
		if (rcv_id == 0)
			broadcastFlag = 1;
				
    if (rcv_id == slaveID || broadcastFlag) { // if the received ID matches the slaveID or broadcasting id (0), continue
      rcv_crc = ((frame[bufferIdx - 2] << 8) | frame[bufferIdx - 1]); // combine the crc Low & High bytes
			
      if (calculateCRC(bufferIdx - 2) == rcv_crc) { // if the calculated crc matches the received crc continue
				function = frame[1];                          // Get function code
				startingAddress = ((frame[2] << 8) | frame[3]); // combine the starting address bytes
				no_of_registers = ((frame[4] << 8) | frame[5]); // combine the number of register bytes
				maxData = startingAddress + no_of_registers;	
				
				//PREPARE RESPONSE MSG
				if (!broadcastFlag && (function == 3)) { // broadcasting is not supported for function 3
					if (startingAddress < holdingRegsSize) { // check exception 2 ILLEGAL DATA ADDRESS
						if (maxData <= holdingRegsSize) { // check exception 3 ILLEGAL DATA VALUE
              noOfBytes = (no_of_registers<<1);
              responseFrameSize = 5 + noOfBytes; // ID, function, noOfBytes, (dataLo + dataHi) * number of registers, crcLo, crcHi
              frame[0] = slaveID;
              frame[1] = function;
              frame[2] = noOfBytes;
              address = 3; // PDU starts at the 4th byte
							for (index = startingAddress; index < maxData; index++) {
								temp = holdingRegs[index];
								frame[address] = temp >> 8; // split the register into 2 bytes
								address++;
								frame[address] = temp & 0xFF;
								address++;
              }
              crc16 = calculateCRC(responseFrameSize - 2);
              frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
              frame[responseFrameSize - 1] = crc16 & 0xFF;
              sendPacket(responseFrameSize);	
							counter++;
            } else exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
          } else exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
        }else
					if (function == 6) { // Preset single Register
						if (startingAddress < holdingRegsSize) { // check exception 2 ILLEGAL DATA ADDRESS
							startingAddress = ((frame[2] << 8) | frame[3]);
							regStatus = ((frame[4] << 8) | frame[5]);
							responseFrameSize = 8;
							holdingRegs[startingAddress] = regStatus;
							crc16 = calculateCRC(responseFrameSize - 2);
              frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
              frame[responseFrameSize - 1] = crc16 & 0xFF;
              sendPacket(responseFrameSize);
            } else
              exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
          } else
							if (function == 16) {
                // check if the recieved number of bytes matches the calculated bytes minus the request bytes
                // id + function + (2 * address bytes) + (2 * no of register bytes) + byte count + (2 * CRC bytes) = 9 bytes
                if (frame[6] == (bufferIdx - 9)) {
                  if (startingAddress < holdingRegsSize) { // check exception 2 ILLEGAL DATA ADDRESS
                     if (maxData <= holdingRegsSize) { // check exception 3 ILLEGAL DATA VALUE
                       address = 7; // start at the 8th byte in the frame
											 for (index = startingAddress; index < maxData; index++) {
                         holdingRegs[index] = ((frame[address] << 8) | frame[address + 1]);
                         address += 2;
                       }
											 // only the first 6 bytes are used for CRC calculation
                       crc16 = calculateCRC(6);
                       frame[6] = crc16 >> 8; // split crc into 2 bytes
                       frame[7] = crc16 & 0xFF;
											 // a function 16 response is an echo of the first 6 bytes from the request + 2 crc bytes
                       if (!broadcastFlag) // don't respond if it's a broadcast message
                           sendPacket(8);
                     } else
                       exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
                 } else
                    exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
               } else
                  errorCount++; // corrupted packet
           } else
               exceptionResponse(1); // exception 1 ILLEGAL FUNCTION
    } else // checksum failed
        errorCount++;
  } // incorrect id
} else 
	if (bufferIdx > 0 && bufferIdx < 6)
      errorCount++; // corrupted packet
return errorCount;
}
//======================================
void exceptionResponse(uint8_t exception){
	uint16_t crc16;
	errorCount++; // each call to exceptionResponse() will increment the errorCount
	if (!broadcastFlag) { // don't respond if its a broadcast message
		frame[0] = slaveID;
		frame[1] = (function | 0x80); // set the MSB bit high, informs the master of an exception
		frame[2] = exception;
		crc16 = calculateCRC(3); // ID, function + 0x80, exception code == 3 bytes
		frame[3] = crc16 >> 8;
		frame[4] = crc16 & 0xFF;
		sendPacket(5); // exception response is always 5 bytes ID, function + 0x80, exception code, 2 bytes crc
	}
}

void modbus_configure(uint32_t _baud, uint8_t _slaveID, uint8_t _TxEnablePin, uint16_t _holdingRegsSize, uint8_t _lowLatency){
	slaveID = _slaveID;
  UART0_Init(_baud);
	
    // Modbus states that a baud rate higher than 19200 must use a fixed 750 us
    // for inter character time out and 1.75 ms for a frame delay.
    // For baud rates below 19200 the timeing is more critical and has to be calculated.
    // E.g. 9600 baud in a 10 bit packet is 960 characters per second
    // In milliseconds this will be 960characters per 1000ms. So for 1 character
    // 1000ms/960characters is 1.04167ms per character and finaly modbus states an
    // intercharacter must be 1.5T or 1.5 times longer than a normal character and thus
    // 1.5T = 1.04167ms * 1.5 = 1.5625ms. A frame delay is 3.5T.
    // Added experimental low latency delays. This makes the implementation
    // non-standard but practically it works with all major modbus master implementations.

    if (_baud == 1000000 && _lowLatency) {
        T1_5 = 1;
        T3_5 = 10;
    } else if (_baud >= 115200 && _lowLatency) {
        T1_5 = 75;
        T3_5 = 175;
    } else if (_baud > 19200) {
        T1_5 = 750;
        T3_5 = 1750;
    } else {
        T1_5 = 15000000/_baud; // 1T * 1.5 = T1.5
        T3_5 = 35000000/_baud; // 1T * 3.5 = T3.5
    }
    holdingRegsSize = _holdingRegsSize;
    errorCount = 0; // initialize errorCount
}

uint16_t calculateCRC(uint8_t bufferSize){
	unsigned int temp, temp2, flag;
	uint8_t i, j;
	temp = 0xFFFF;
	for (i = 0; i < bufferSize; i++){
		temp = temp ^ frame[i];
		for (j = 0; j < 8; j++){
			flag = temp & 0x0001;
			temp >>= 1;
			if (flag) temp ^= 0xA001;
    }
  }
	// Reverse byte order.
	temp2 = temp >> 8;
	temp = (temp << 8) | temp2;
	temp &= 0xFFFF;
	return temp; // the returned value is already swapped - crcLo byte is first & crcHi byte is last
}

void sendPacket(uint8_t bufferSize)
{
	uint8_t i;
	RS485_TRANSMIT;
	for ( i = 0; i < bufferSize; i++)
		UART_Write(UART0, frame+i,1);
	// allow a frame delay to indicate end of transmission
	CLK_SysTickDelay(T3_5);
	RS485_RECEIVER;
}
void UART0_Init(uint32_t baudrate){
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
		// UART_Open(UART0, baudrate);
	 UART_SetLine_Config(UART0, baudrate,UART_WORD_LEN_8,UART_PARITY_EVEN,UART_STOP_BIT_1);
}
