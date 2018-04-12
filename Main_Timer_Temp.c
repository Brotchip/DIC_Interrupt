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
							V2.00  12.04.2018
							Timer 
							
*/
/******************************************************************************/
#include <stm32f10x.h>
#include <armv10_std.h>


/*------------------------------ Function Prototypes -------------------------*/
static void TIM1_Config(void);
static void NVIC_init(char position, char priority); 

/*------------------------------ Static Variables-------------------------*/
static int sekunden;

/******************************************************************************/
/*           Interrupt Service Routine  Timer1 (General Purpose Timer)        */
/******************************************************************************/
void TIM1_UP_IRQHandler(void)	//Timer 1, l�st alle 1000ms aus
{
	TIM1->SR &=~0x01;	 //Interrupt pending bit l�schen (Verhinderung eines nochmaligen Interrupt-ausl�sens)
	sekunden++;
}

/******************************************************************************/
/*                     Initialization Timer1 (General Purpose Timer)          */
/******************************************************************************/
static void TIM1_Config(void)
{	
	/*---------------------- Timer 2 konfigurieren -----------------------*/
	RCC->APB1ENR |= 0x0001;	//Timer 2 Clock enable
	TIM1->SMCR = RCC_APB1ENR_TIM2EN;	//Timer 1 Clock Selection: CK_INT wird verwendet
	TIM1->CR1  = 0x0000;	// Auswahl des Timer Modus: Upcounter --> Z�hlt: von 0 bis zum Wert des Autoreload-Registers

	/* T_INT = 126,26ns, Annahme: Presc = 3 ---> Auto Reload Wert = 18 (=0x12) --> 1us Update Event*/
	TIM1->PSC = 3;		//Wert des prescalers (Taktverminderung)
	TIM1->ARR = 0x12;		//Auto-Reload Wert = Maximaler Zaehlerstand des Upcounters
	TIM1->RCR = 0;			//Repetition Counter deaktivieren

	/*--------------------- Interrupt Timer 1 konfigurieren ---------------------*/
	TIM1->DIER = 0x01;	   //enable Interrupt bei einem UEV (�berlauf / Unterlauf)
	NVIC_init(TIM1_UP_IRQn,2);	   //Enable Timer 1 Update Interrupt, Priority 2

	/*-------------------------- Timer 1 Starten -------------------------------*/
    TIM1->CR1 |= 0x0001;   //Counter-Enable bit setzen

}

/******************************************************************************/
/*                   NVIC_init(char position, char priority)    			  */ 
/* Funktion:                                                                  */    
/*   �bernimmt die vollst�ndige initialisierung eines Interrupts  im Nested   */
/*   vectored Interrupt controller (Priorit�t setzen, Ausl�sen verhindern,    */
/*   Interrupt enablen)                                                       */
/* �bergabeparameter: "position" = 0-67 (Nummer des Interrupts)               */
/*                    "priority": 0-15 (Priorit�t des Interrupts)		      */
/******************************************************************************/
static void NVIC_init(char position, char priority) 
{	
	NVIC->IP[position]=(priority<<4);	//Interrupt priority register: Setzen der Interrupt Priorit�t
	NVIC->ICPR[position >> 0x05] |= (0x01 << (position & 0x1F));//Interrupt Clear Pendig Register: Verhindert, dass der Interrupt ausl�st sobald er enabled wird 
	NVIC->ISER[position >> 0x05] |= (0x01 << (position & 0x1F));//Interrupt Set Enable Register: Enable interrupt
}

/******************************************************************************/
/*                                MAIN function                               */
/******************************************************************************/
int main (void) {
		int temp;

	
		//PB1 auf Alternate Funktion Open Drain setzen
		RCC->APB2ENR |= 0x8;			//Takt f�r GPIOB aktivieren
		temp = GPIOB->CRL;
		temp &= 0x00000000;				// PB7-PB0 Konfigurationsbits l�schen
		temp |= 0x000000D0;				// PB1 als Alternate Funktion Open Drain setzen
		GPIOB->CRL = temp;
	
		
	
	}