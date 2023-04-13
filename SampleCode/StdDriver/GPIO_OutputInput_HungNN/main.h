#include <stdio.h>
#include "M051Series.h"
#include "SimpleModbusSlave.h"


#define PLL_CLOCK           50000000

#define RL1 P10
#define RL2 P11
#define RL3 P12
#define RL4 P13
#define RL5 P14
#define RL6 P15
#define RL7 P16
#define RL8 P17
#define RL9 P20
#define RL10 P21
#define RL11 P22
#define RL12 P23
#define RL13 P24
#define RL14 P25
#define RL15 P26
#define RL16 P27

#define KA1 P20
#define KA2 P21
#define KA3 P22
#define KA4 P23
#define KA5 P24
#define KA6 P25
#define KA7 P26
#define KA8 P27
#define KA9 P40
#define KA10 P41
#define KA11 P42
#define KA12 P43
#define KA13 P44
#define KA14 P45
#define KA15 P35
#define KA16 P36

#define KB1 P00
#define KB2 P01
#define KB3 P02
#define KB4 P03
#define KB5 P04
#define KB6 P05
#define KB7 P06
#define KB8 P07
#define KB9 P00
#define KB10 P10
#define KB11 P11
#define KB12 P12
#define KB13 P13
#define KB14 P14
#define KB15 P15
#define KB16 P16

#define E1 P23
#define E2 P24
#define E3 P25
#define E4 P26
#define E5 P27

#define DIR485 P32 



#define Relay_ON(Pin)  ((Pin) = FALSE)
#define Relay_OFF(Pin)  ((Pin) = TRUE)

#define KA_L ((KA8 <<7)|(KA7 <<6)|(KA6 <<5)|(KA5 <<4)|(KA4 <<3)|(KA3 <<2)|(KA2 <<1)|(KA1 <<0))
#define KA_H ((KA16 <<7)|(KA15 <<6)|(KA14 <<5)|(KA13 <<4)|(KA12 <<3)|(KA11 <<2)|(KA10 <<1)|(KA9 <<0))
#define KA ((KA_H<<8)|KA_L)

#define KB_L ((KB8 <<7)|(KB7 <<6)|(KB6 <<5)|(KB5 <<4)|(KB4 <<3)|(KB3 <<2)|(KB2 <<1)|(KB1 <<0))
#define KB_H ((KB16 <<7)|(KB15 <<6)|(KB14 <<5)|(KB13 <<4)|(KB12 <<3)|(KB11 <<2)|(KB10 <<1)|(KB9 <<0))
#define KB ((KB_H<<8)|KB_L)


/* Auto add more MB register*/
enum{  // Bo sung them cac thanh ghi Holding Regiters
	Relay_HoldingRegs_01, // 0
	Relay_HoldingRegs_11, // 1
	Relay_HoldingRegs_12, // 2
	Relay_HoldingRegs_13, // 3
	Relay_HoldingRegs_14, // 4
	Relay_HoldingRegs_21, // 5
	Relay_HoldingRegs_22, // 6
	Relay_HoldingRegs_23, // 7
	Relay_HoldingRegs_24, // 8
	Relay_HoldingRegs_30, // 9
	Relay_HoldingRegs_31, // 10		
	Relay_HoldingRegs_40, // 11   Reserved
	Relay_HoldingRegs_41, // 12	 Reserved
	Relay_HoldingRegs_42, //  13	 Reserved
	Relay_HoldingRegs_43, //  14	 Reserved
	Relay_HoldingRegs_44, //  15	 Reserved
	Relay_HoldingRegs_45, //  16	 Reserved
	Opto_HoldingRegs_50_1, // 17 Opto 1-16
	Opto_HoldingRegs_50_2, // 18 Opto 17-32
	Opto_HoldingRegs_51_1, // 19 Opto 33-48
	Opto_HoldingRegs_51_2, // 20 Opto 49- 64
	Opto_HoldingRegs_52_1, // 21  Reserved
	Opto_HoldingRegs_52_2, // 22  Reserved
	Opto_HoldingRegs_53_1, // 23  Reserved
	Opto_HoldingRegs_53_2, // 24  Reserved
	temp_status,           // 25 COUNTER
	// add more Regs before this line=======
	HoldingRegsSize
};

// HoldingRegs dung de luu du lieu cap nhat relay và optos
uint16_t HoldingRegs[HoldingRegsSize]; 

void SYS_Init(void);
void Relay_init(void);
void Optos_init(void);
void Address_init(void);
uint8_t ModbusAddressRead(void);
void Delay(uint32_t DLY);
void Relay_Update(const uint8_t X1, const uint8_t X2);


