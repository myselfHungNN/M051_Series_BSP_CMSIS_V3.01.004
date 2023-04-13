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
#define KB9 P10
#define KB10 P11
#define KB11 P12
#define KB12 P13
#define KB13 P14
#define KB14 P15
#define KB15 P16
#define KB16 P17

#define KC1 P20
#define KC2 P21
#define KC3 P22
#define KC4 P23
#define KC5 P24
#define KC6 P25
#define KC7 P26
#define KC8 P27
#define KC9 P40
#define KC10 P41
#define KC11 P42
#define KC12 P43
#define KC13 P44
#define KC14 P45
#define KC15 P35
#define KC16 P36

#define KD1 P00
#define KD2 P01
#define KD3 P02
#define KD4 P03
#define KD5 P04
#define KD6 P05
#define KD7 P06
#define KD8 P07
#define KD9 P10
#define KD10 P11
#define KD11 P12
#define KD12 P13
#define KD13 P14
#define KD14 P15
#define KD15 P16
#define KD16 P17

#define E1 P23
#define E2 P24
#define E3 P25
#define E4 P26
#define E5 P27

#define DIR485 P32 

#ifdef OPTOS
	#define opto1  KA2
	#define opto7  KA4
	#define opto13 KA6
	#define opto19 KA8
	#define opto25 KA10
	#define opto31 KA12
	#define opto37 KA14
	#define opto43 KA16	
	
	#define opto2  KA1
	#define opto8  KA3
	#define opto14 KA5
	#define opto20 KA7
	#define opto26 KA9
	#define opto32 KA11
	#define opto38 KA13
	#define opto44 KA15
	
	#define opto3  KB1
	#define opto9  KB3
	#define opto15 KB5
	#define opto21 KB7
	#define opto27 KB9
	#define opto33 KB11
	#define opto39 KB13
	#define opto45 KB15
	
	#define opto4  KB2
	#define opto10 KB4
	#define opto16 KB6
	#define opto22 KB8
	#define opto28 KB10
	#define opto34 KB12
	#define opto40 KB14
	#define opto46 KB16
	
	#define KA_L ((opto10<<7)|(opto9<<6)|(opto8<<5)|(opto7<<4)|(opto4<<3)|(opto3<<2)|(opto2<<1)|(opto1<<0))
	#define KA_H ((opto22<<7)|(opto21<<6)|(opto20<<5)|(opto19<<4)|(opto16<<3)|(opto15<<2)|(opto14<<1)|(opto13<<0))
	#define KB_L ((opto34<<7)|(opto33<<6)|(opto32<<5)|(opto31<<4)|(opto28<<3)|(opto27<<2)|(opto26<<1)|(opto25<<0))
	#define KB_H ((opto46<<7)|(opto45<<6)|(opto44<<5)|(opto43<<4)|(opto40<<3)|(opto39<<2)|(opto38<<1)|(opto37<<0))	
#endif

#ifdef OPTOS
	#define opto49  KC2
	#define opto55  KC4
	#define opto5   KC6
	#define opto11  KC8
	#define opto17  KC10
	#define opto23  KC12
	#define opto29  KC14
	#define reserved1 KC16
	
	#define reserved3 KC1
	#define opto50  KC3
	#define opto56  KC5
	#define opto35  KC7
	#define opto41  KC9
	#define opto47  KC11
	#define opto53  KC13
	#define opto59  KC15
	
	#define opto51  KD1
	#define opto57  KD3
	#define opto6   KD5
	#define opto12  KD7
	#define opto18  KD9
	#define opto24  KD11
	#define opto30  KD13
	#define reserved2 KD15
	
	#define reserved4  KD2
	#define opto52  KD4
	#define opto58 KD6
	#define opto36 KD8
	#define opto42 KD10
	#define opto48 KD12
	#define opto54 KD14
	#define opto60 KD16
	
	#define KC_L ((opto24<<7)|(opto23<<6)|(opto18<<5)|(opto17<<4)|(opto12<<3)|(opto11<<2)|(opto6<<1)|(opto5<<0))
	#define KC_H ((opto48<<7)|(opto47<<6)|(opto42<<5)|(opto41<<4)|(opto36<<3)|(opto35<<2)|(opto30<<1)|(opto29<<0))
	#define KD_L ((opto56<<7)|(opto55<<6)|(opto54<<5)|(opto53<<4)|(opto52<<3)|(opto51<<2)|(opto50<<1)|(opto49<<0))
	#define KD_H ((reserved4<<7)|(reserved3<<6)|(reserved2<<5)|(reserved1<<4)|(opto60<<3)|(opto59<<2)|(opto58<<1)|(opto57<<0))	
#endif

#define KA ((KA_H<<8)|KA_L)
#define KB ((KB_H<<8)|KB_L)

#define KC ((KC_H<<8)|KC_L)
#define KD ((KD_H<<8)|KD_L)

#define Relay_ON(Pin)  ((Pin) = FALSE)
#define Relay_OFF(Pin)  ((Pin) = TRUE)
/*
#define KA_L ((KA8 <<7)|(KA7 <<6)|(KA6 <<5)|(KA5 <<4)|(KA4 <<3)|(KA3 <<2)|(KA2 <<1)|(KA1 <<0))
#define KA_H ((KA16 <<7)|(KA15 <<6)|(KA14 <<5)|(KA13 <<4)|(KA12 <<3)|(KA11 <<2)|(KA10 <<1)|(KA9 <<0))
#define KA ((KA_H<<8)|KA_L)

#define KB_L ((KB8 <<7)|(KB7 <<6)|(KB6 <<5)|(KB5 <<4)|(KB4 <<3)|(KB3 <<2)|(KB2 <<1)|(KB1 <<0))
#define KB_H ((KB16 <<7)|(KB15 <<6)|(KB14 <<5)|(KB13 <<4)|(KB12 <<3)|(KB11 <<2)|(KB10 <<1)|(KB9 <<0))
#define KB ((KB_H<<8)|KB_L)
*/

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
	
	Opto_HoldingRegs_50_1, // 17 
	Opto_HoldingRegs_50_2, // 18 
	Opto_HoldingRegs_51_1, // 19 
	Opto_HoldingRegs_51_2, // 20 
	
	Opto_HoldingRegs_52_1, // 21  Reserved
	Opto_HoldingRegs_52_2, // 22  Reserved
	Opto_HoldingRegs_53_1, // 23  Reserved
	Opto_HoldingRegs_53_2, // 24  Reserved
	counterIdx,
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


