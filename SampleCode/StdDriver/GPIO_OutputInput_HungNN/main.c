/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * $Revision: 4 $
 * $Date: 15/05/22 2:51p $
 * @brief    Show how to set GPIO pin mode and use pin data input/output control.
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include "main.h"

extern uint16_t counter; // demostration: MBfunction 0x03
const uint8_t BoardID = 45;
#define INDEX Relay_HoldingRegs_45


/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void){
	
	uint8_t X1 = 255,X2 = 255;
	
  /* Unlock protected registers */
  SYS_UnlockReg();

  /* Init System, peripheral clock and multi-function I/O */
  SYS_Init();
  /* Lock protected registers */
  SYS_LockReg();

  /* Init Modbus RTU */
	modbus_configure(9600, BoardID, 32, HoldingRegsSize, 0);
	
	/* Main loop*/
	while (TRUE){
		// wait MB command from PC apps
		modbus_update(HoldingRegs);	
		
		// Update DI to Holding Register		
		 HoldingRegs[Opto_HoldingRegs_50_1] = Opto_1;
		 HoldingRegs[Opto_HoldingRegs_50_2] = Opto_2;
		//HoldingRegs[Opto_HoldingRegs_51_1] = Opto_1;
		//HoldingRegs[Opto_HoldingRegs_51_2] = Opto_2;
		
		// Update Relay from Master		
		X1 = HoldingRegs[INDEX] & 0xFF;
		X2 = HoldingRegs[INDEX] >> 8;	
		Relay_Update(X1,X2);		
		
		CLK_SysTickDelay(100); // 100us
	}
}
void SYS_Init(void){
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));		  

    /* Enable external XTAL 12MHz clock */
    // CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    // CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
		SystemCoreClockUpdate(); // Update CyclesPerUs val for  fcn:CLK_SysTickDelay(us)
	
		/* set SysTick Timer Interupt Period - HungNN */
		// CLK_EnableSysTick(CLK_CLKSEL0_STCLK_S_HIRC_DIV2, 0xFFFF);
		
    /* Enable UART module clock */		
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set P3 multi-function pins for UART0 RXD and TXD */
    SYS->P3_MFP &= ~(SYS_MFP_P30_Msk | SYS_MFP_P31_Msk);
    SYS->P3_MFP |= (SYS_MFP_P30_RXD0 | SYS_MFP_P31_TXD0);		
		// RS485 DIR
		GPIO_SetMode(P3, BIT2, GPIO_PMD_OUTPUT); RS485_RECEIVER;
		
		/* Relay Board*/		
		// Relay_init();
		
		/* Optos Board*/
		 Optos_init();
		/* Get Board ID */
		// ModbusAddressRead();	
}
void Address_init(void){
	GPIO_SetMode(P0, BIT3, GPIO_PMD_INPUT); // E1
	GPIO_SetMode(P0, BIT4, GPIO_PMD_INPUT); // E2
	GPIO_SetMode(P0, BIT5, GPIO_PMD_INPUT); // E3
	GPIO_SetMode(P0, BIT6, GPIO_PMD_INPUT); // E4
	GPIO_SetMode(P0, BIT7, GPIO_PMD_INPUT); // E5
}

void Relay_init(){
	GPIO_SetMode(P1, BIT0, GPIO_PMD_OUTPUT); Relay_OFF(RL1);
	GPIO_SetMode(P1, BIT1, GPIO_PMD_OUTPUT); Relay_OFF(RL2);
	GPIO_SetMode(P1, BIT2, GPIO_PMD_OUTPUT); Relay_OFF(RL3);
	GPIO_SetMode(P1, BIT3, GPIO_PMD_OUTPUT); Relay_OFF(RL4);
	GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT); Relay_OFF(RL5);
	GPIO_SetMode(P1, BIT5, GPIO_PMD_OUTPUT); Relay_OFF(RL6);
	GPIO_SetMode(P1, BIT6, GPIO_PMD_OUTPUT); Relay_OFF(RL7);
	GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT); Relay_OFF(RL8);
	GPIO_SetMode(P2, BIT0, GPIO_PMD_OUTPUT); Relay_OFF(RL9);
	GPIO_SetMode(P2, BIT1, GPIO_PMD_OUTPUT); Relay_OFF(RL10);
	GPIO_SetMode(P2, BIT2, GPIO_PMD_OUTPUT); Relay_OFF(RL11);
	GPIO_SetMode(P2, BIT3, GPIO_PMD_OUTPUT); Relay_OFF(RL12);
	GPIO_SetMode(P2, BIT4, GPIO_PMD_OUTPUT); Relay_OFF(RL13);
	GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT); Relay_OFF(RL14);
	GPIO_SetMode(P2, BIT6, GPIO_PMD_OUTPUT); Relay_OFF(RL15);
	GPIO_SetMode(P2, BIT7, GPIO_PMD_OUTPUT); Relay_OFF(RL16);
}
void Optos_init(){
	GPIO_SetMode(P2, BIT0, GPIO_PMD_INPUT); 
	GPIO_SetMode(P2, BIT1, GPIO_PMD_INPUT);
	GPIO_SetMode(P2, BIT2, GPIO_PMD_INPUT);
	GPIO_SetMode(P2, BIT3, GPIO_PMD_INPUT);
	GPIO_SetMode(P2, BIT4, GPIO_PMD_INPUT);
	GPIO_SetMode(P2, BIT5, GPIO_PMD_INPUT);
	GPIO_SetMode(P2, BIT6, GPIO_PMD_INPUT);
	GPIO_SetMode(P2, BIT7, GPIO_PMD_INPUT);
	
	GPIO_SetMode(P4, BIT0, GPIO_PMD_INPUT);
	GPIO_SetMode(P4, BIT1, GPIO_PMD_INPUT);
	GPIO_SetMode(P4, BIT2, GPIO_PMD_INPUT);
	GPIO_SetMode(P4, BIT3, GPIO_PMD_INPUT);
	GPIO_SetMode(P4, BIT4, GPIO_PMD_INPUT);
	GPIO_SetMode(P4, BIT5, GPIO_PMD_INPUT);
	GPIO_SetMode(P3, BIT5, GPIO_PMD_INPUT);
	GPIO_SetMode(P3, BIT6, GPIO_PMD_INPUT);
	
	GPIO_SetMode(P0, BIT0, GPIO_PMD_INPUT);
	GPIO_SetMode(P0, BIT1, GPIO_PMD_INPUT);
	GPIO_SetMode(P0, BIT2, GPIO_PMD_INPUT);
	GPIO_SetMode(P0, BIT3, GPIO_PMD_INPUT);
	GPIO_SetMode(P0, BIT4, GPIO_PMD_INPUT);
	GPIO_SetMode(P0, BIT5, GPIO_PMD_INPUT);
	GPIO_SetMode(P0, BIT6, GPIO_PMD_INPUT);
	GPIO_SetMode(P0, BIT7, GPIO_PMD_INPUT);
	
	GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);
	GPIO_SetMode(P1, BIT1, GPIO_PMD_INPUT);
	GPIO_SetMode(P1, BIT2, GPIO_PMD_INPUT);
	GPIO_SetMode(P1, BIT3, GPIO_PMD_INPUT);
	GPIO_SetMode(P1, BIT4, GPIO_PMD_INPUT);
	GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);
	GPIO_SetMode(P1, BIT6, GPIO_PMD_INPUT);
	GPIO_SetMode(P1, BIT7, GPIO_PMD_INPUT);
}

void Delay(uint32_t DLY){
	uint32_t temp1, temp2;
	for (temp1 = 0; temp1 < DLY; temp1++)
		for (temp2 = 0; temp2 < 10000; temp2++);
}
void Relay_Update(const uint8_t X1, const uint8_t X2){
	static uint8_t X1_old, X2_old; // Hung note them
	
	uint8_t bit_tmp;
	
	bit_tmp = (X1>>0)%2;
	if (bit_tmp) 		Relay_ON(RL1);
	else Relay_OFF(RL1);
	bit_tmp = (X1>>1)%2;
	if (bit_tmp) 		Relay_ON(RL2);
	else Relay_OFF(RL2);
	bit_tmp = (X1>>2)%2;
	if (bit_tmp) 		Relay_ON(RL3);
	else Relay_OFF(RL3);
	bit_tmp = (X1>>3)%2;
	if (bit_tmp) 		Relay_ON(RL4);
	else Relay_OFF(RL4);
	bit_tmp = (X1>>4)%2;
	if (bit_tmp) 		Relay_ON(RL5);
	else Relay_OFF(RL5);
	bit_tmp = (X1>>5)%2;
	if (bit_tmp) 		Relay_ON(RL6);
	else Relay_OFF(RL6);
	bit_tmp = (X1>>6)%2;
	if (bit_tmp) 		Relay_ON(RL7);
	else Relay_OFF(RL7);
	bit_tmp = (X1>>7)%2;
	if (bit_tmp) 		Relay_ON(RL8);
	else Relay_OFF(RL8);
	bit_tmp = (X2>>0)%2;
	if (bit_tmp) 		Relay_ON(RL9);
	else Relay_OFF(RL9);
	bit_tmp = (X2>>1)%2;
	if (bit_tmp) 		Relay_ON(RL10);
	else Relay_OFF(RL10);
	bit_tmp = (X2>>2)%2;
	if (bit_tmp) 		Relay_ON(RL11);
	else Relay_OFF(RL11);
	bit_tmp = (X2>>3)%2;
	if (bit_tmp) 		Relay_ON(RL12);
	else Relay_OFF(RL12);
	bit_tmp = (X2>>4)%2;
	if (bit_tmp) 		Relay_ON(RL13);
	else Relay_OFF(RL13);
	bit_tmp = (X2>>5)%2;
	if (bit_tmp) 		Relay_ON(RL14);
	else Relay_OFF(RL14);
	bit_tmp = (X2>>6)%2;
	if (bit_tmp) 		Relay_ON(RL15);
	else Relay_OFF(RL15);
	bit_tmp = (X2>>7)%2;
	if (bit_tmp) 		Relay_ON(RL16);
	else Relay_OFF(RL16);
	
}
uint8_t read_MBaddress(void){
	uint8_t  temp;
	temp = (E5<<4)|(E4<<3)|(E3<<2)|(E2<<1)|(E1<<0);
	return temp;
}


/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
