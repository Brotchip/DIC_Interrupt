/******************************************************************************/
/*   (C) Copyright HTL - HOLLABRUNN  2009-2009 All rights reserved. AUSTRIA   */ 
/*                                                                            */ 
/* File Name:   Lauflicht_Interrupt.c                                         */
/* Autor: 		Josef Reisinger                                               */
/* Version: 	V1.00                                                         */
/* Date: 		11/03/2012                                                    */
/* Description: Demoprogramm f�r Timer 1                                      */
/******************************************************************************/
/* History: 	V1.00  creation										          */
/******************************************************************************/
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
	/*---------------------- Timer 1 konfigurieren -----------------------*/
	RCC->APB2ENR |= 0x0800;	//Timer 1 Clock enable
	TIM1->SMCR = 0x0000;	//Timer 1 Clock Selection: CK_INT wird verwendet
	TIM1->CR1  = 0x0000;	// Auswahl des Timer Modus: Upcounter --> Z�hlt: von 0 bis zum Wert des Autoreload-Registers

	/* T_INT = 126,26ns, Annahme: Presc = 150 ---> Auto Reload Wert = 52801 (=0xCE41) --> 1s Update Event*/
	TIM1->PSC = 150;		//Wert des prescalers (Taktverminderung)
	TIM1->ARR = 0xCE41;		//Auto-Reload Wert = Maximaler Zaehlerstand des Upcounters
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
int main (void) 
{
int lauflicht; 			// Lauflicht P1 Abbild
int lcd_sekunden; 		// aktueller Z�hlerstand auf LCD
char buffer[30];

	init_leds_switches();
  	uart_init(9600);    // 9600,8,n,1
  	uart_clear();       // Send Clear String to VT 100 Terminal
  	lcd_init();         // LCD initialisieren
	lcd_clear();        // L�sche LCD Anzeige
  	lauflicht = 0x01;	// Lauflicht im Speicher initilalisieren		
  	sekunden=0; 		// Sekundenzaehler initialisieren
  	lcd_sekunden = 0;	// aktueller Anzeigewert
  	set_leds(0);        // LED l�schen
	TIM1_Config();	    // Timer 1 starten: Upcounter --> l�st alle 1s einen Update Interrupt
	uart_put_string("Lauflicht mit Interrupt:\n");
	do
  	{
		if (lcd_sekunden != sekunden)    // 1 Sekunde vergangen ?
	 	{ 
	   		lcd_sekunden = sekunden;
	   		set_leds(lauflicht);           // Lauflicht auf LED aktualisieren
	   		lauflicht = lauflicht*2;       // naechtes Lauflicht Bit
	   		if (lauflicht==256)            // letztes Lauflicht Bit ?
	   		{
	     		lauflicht = 0x01;          // Anfangszustand Lauflicht
	   		}  
	 		lcd_set_cursor(0,0);                 // Cursor auf Ursprung
			sprintf(&buffer[0],"Sekunden=%d", lcd_sekunden); // Sekundenzaehler auf LCD aktualisieren
    		lcd_put_string(&buffer[0]);
		}
  	} while (1);

}
