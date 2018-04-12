/******************************************************************************/
/*   (C) Copyright Jamboeck/Trauner  2018-2018 All rights reserved. AUSTRIA   */ 
/*                                                                            */ 
/* File Name: Main_Timer_Temp                                                    */
/* Autor: 		Jamboeck Thomas, Trauner Michael                              */
/* Version: 	V1.00                                                         */
/* Date: 		  05.04.2018                                               */
/* Description: Timer mit Temperatursensor Interrupt                        */
/******************************************************************************/
/* History: 	V1.00  05.04.2018
							creation	
							
*/
/******************************************************************************/
#include <stm32f10x.h>
#include <armv10_std.h>


/******************************************************************************/
/*                                MAIN function                               */
/******************************************************************************/
int main (void) {
		int temp;
	
		//PB1 auf Alternate Funktion Open Drain setzen
		RCC->APB2ENR |= 0x8;			//Takt für GPIOB aktivieren
		temp = GPIOB->CRL;
		temp &= 0x00000000;				// PB7-PB0 Konfigurationsbits löschen
		temp |= 0x000000D0;				// PB1 als Alternate Funktion Open Drain setzen
		GPIOB->CRL = temp;
	
		wait_ms(100);
	}