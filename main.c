/***************************************************
 * This is an example program on how to use the 
 * ST7735R based 1.8" color TFT LCD display from
 * Adafruit Industries with the Atmel ATmega328PB
 * Xplained mini developer board.
 * Note that this display also contains a micro SD
 * card socket on its back side.
 * 
 * See here for further information:
 * http://www.adafruit.com/products/358
 * 
 * This code is under the MIT licence.
 * 
 ****************************************************/
/*/##### CONEXIONES TFT DISPLAY ####
D/C --> PB0
RESET --> PB1
CS-TFT --> PB2
MOSI --> PB3
SCK --> PB5
//########*/

// MCU Clock Speed - needed for delay.h
#define F_CPU	16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


#include "SPI/SPI_implement_me.h"
#include "USART/USART_implement_me.h"

#include "display/ST7735_commands.h"
#include "display/graphic_shapes.h"
#include "display/testroutines.h"


// UART configuration
#define BAUD	57600					// serial communication baud rate
#define UBRR_VALUE F_CPU/16/BAUD-1
static volatile char Rx;
void dibCuad(int x, int y);
void borCuad(int x, int y);
void actTablero(int a[12][9]);
void dibFig(int vert, int hor, int rot, int tipo);

void dibCuad(int x, int y){
	fillRect(13+8*x,22+8*y,7,7,ST7735_WHITE);
}

void borCuad(int x, int y){
	fillRect(13+8*x,22+8*y,7,7,ST7735_BLACK);
}

void actTablero(int a[12][9]){
	for (int x=0; x<8; x=x+1){
		
		for (int y=0; y<11; y=y+1){
			if (a[y][x]==1){
				dibCuad(x,y);
			}
			else {borCuad(x,y);}
			
		}	
		
	}
	
}
//funcion que dibuja la figura que está bajando acorde a su posición y rotacion
void dibFig(int hori, int vert, int rot, int tipo){ 
			if (tipo==1){
				if (rot==1){
					dibCuad(hori,vert);
					dibCuad(hori+1,vert);
					dibCuad(hori+2,vert);
					dibCuad(hori,vert+1);
				}
				if (rot==2){
					dibCuad(hori,vert);
					dibCuad(hori,vert+1);
					dibCuad(hori,vert+2);
					dibCuad(hori+1,vert+2);}
				if (rot==3){
					dibCuad(hori,vert+1);
					dibCuad(hori+1,vert+1);
					dibCuad(hori+2,vert+1);
					dibCuad(hori+2,vert);
				}
				if (rot==4){
					dibCuad(hori,vert);
					dibCuad(hori+1,vert);
					dibCuad(hori+1,vert+1);
					dibCuad(hori+1,vert+2);
				}
			 }
			 
			if (tipo==2){
				if (rot==1){
					dibCuad(hori,vert);
					dibCuad(hori+1,vert);
					dibCuad(hori+2,vert);
				}
				if (rot==2){
					dibCuad(hori,vert);
					dibCuad(hori,vert+1);
					dibCuad(hori,vert+2);}
				if (rot==3){
					dibCuad(hori,vert);
					dibCuad(hori+1,vert);
					dibCuad(hori+2,vert);
				}
				if (rot==4){
					dibCuad(hori,vert);
					dibCuad(hori,vert+1);
					dibCuad(hori,vert+2);
				}
			 }	
}
//función que escribe una figura en el tablero cuando ya no puede bajar más 
int writeTablero(int a[12][9], int hori, int vert, int rot, int fig){
	if (fig == 1){
		if (rot == 1){
			a[vert][hori]=1;
			a[vert][hori+1]=1;
			a[vert][hori+2]=1;
			a[vert+1][hori]=1;
		}
		if (rot == 2){
			a[vert][hori]=1;
			a[vert][hori]=1;
			a[vert][hori]=1;
			a[vert][hori]=1;
		}
		if (rot == 3){
			a[vert][hori]=1;
			a[vert][hori]=1;
			a[vert][hori]=1;
			a[vert][hori]=1;
		}
		if (rot == 4){
			a[vert][hori]=1;
			a[vert][hori]=1;
			a[vert][hori]=1;
			a[vert][hori]=1;
		}
	}
}
	

int tablero[12][9] ={
	{0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,1},
	{0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1},
};

int rotacion = 1;
int horizontal = 1;
int vertical = 1;

/** The main function */
int main(void)
{
	// Initialises the serial communication interface
	struct USART_configuration config_57600_8N1 = {57600, 8, 0, 1};
	USART_Init(config_57600_8N1);
	
	//PIN INTERRUPTS PARA CONTROLES
	//ROTACION
	
	//DDRD &= ~(1 << DDD2);     // Clear the PD2 pin
	// PD2 (PCINT0 pin) is now an input
	//PORTD = ~(1 << PORTD2);    // turn On the Pull-up
	// PD2 is now an input with pull-up enabled
	//EICRA |= (1 << ISC00)|(1 << ISC01);    // set INT0 to trigger on ANY logic change
	//EIMSK |= (1 << INT0);     // Turns on INT0
	
	// MOVIMIENTO A LA DERECHA
	
	//DDRD &= ~(1 << DDD4);     // Clear the PD2 pin
	// PD2 (PCINT0 pin) is now an input
	//PORTD = ~(1 << PORTD4);    // turn On the Pull-up
	// PD2 is now an input with pull-up enabled
	//EICRA |= (1 << ISC10)|(1 << ISC11);    // set INT0 to trigger on ANY logic change
	//EIMSK |= (1 << INT1);     // Turns on INT0
	

	
	//CONFIGURACIÓN DE TIMER PARA BAJADA DE LAS FIGURAS
		TCNT1 = 50000;   // for 1 sec at 16 MHz // modificar para ajustar dificultad

		TCCR1A = 0x00;
		TCCR1B = (1<<CS10) | (1<<CS12);;  // Timer mode with 1024 prescler
		TIMSK1 = (1 << TOIE1) ;   // Enable timer1 overflow interrupt(TOIE1)
		
	// Welcome message
	USART_Transmit_String("Bienvenido a TETRIS\r\n\n");
		
	
	SPI_Master_Init();
	ST7735_init();
	sei();
	
	fillScreen(ST7735_BLACK);
		
	while(true)
	{		
		USART_Transmit_String("Baja Figura\r\n\n");	
		

		
		//DIBUJA TABLERO DE JUEGO
		
		drawRect(87,6+8,72,15,ST7735_WHITE);
		drawtext(18,2,"TETRIS", ST7735_WHITE, ST7735_BLACK,1);
		drawtext(18,5,"SCORE", ST7735_WHITE, ST7735_BLACK,1);
		drawRect(87,6+32+4,72,30,ST7735_WHITE);
		drawtext(16,9,"MAX SCORE", ST7735_WHITE, ST7735_BLACK,1);
		drawRect(87,6+32+32+8,72,30,ST7735_WHITE);
		for (int x=0; x<10; x=x+1){fillRect(5+8*x,6+8,7,7,ST7735_WHITE);
			                      fillRect(5+8*x,102+8,7,7,ST7735_WHITE);}
								  
		for (int x=0; x<12; x=x+1){fillRect(5,6+8*x+8,7,7,ST7735_WHITE);
		fillRect(5+8*9,6+8*x+8,7,7,ST7735_WHITE);}
		
		actTablero(tablero);
		
	
		//END TABLERO
		
		dibFig(horizontal,vertical,rotacion,1);
		
		// Here would be a nice place for drawing a bitmap, wouldn't it?
		



	}
}

// Tx Complete
ISR(USART_TX_vect)
{
	// Do nothing - simply here to enable a breakpoint in the ISR
}

//Recepción de comando //mueve figura acorde al comando recibido
ISR(USART_RX_vect)
{
	 Rx = USART_Receive_char();
	 
	 if (Rx == 49){
	               horizontal = horizontal - 1; if (horizontal < 0){horizontal=0;}}
	 
     if (Rx == 50){
                   horizontal = horizontal + 1; if (horizontal > 7){horizontal=7;}}
	 if (Rx == 51){if (rotacion == 4){rotacion=0;}
					rotacion = rotacion+1;}
	 if (Rx == 52){//VOLVER A MENU
	 }
	 USART_Transmit_String("Recibido:");
	 USART_Transmit_String(Rx);
}

//ISR (INT0_vect)
//{	
//	if (rotacion == 4){rotacion=0;}
//	rotacion = rotacion+1;
//	
//}

//ISR (INT1_vect)
//{
//	if (horizontal > 7){horizontal=0;}
//	horizontal = horizontal + 1;
//	
//}

ISR (TIMER1_OVF_vect)    // Timer1 ISR
{
	vertical = vertical + 1;
	if (vertical == 9){writeTablero(tablero,horizontal,vertical,rotacion,1);}
	if (vertical == 10){vertical = 0;}
	TCNT1 = 50000;   // for 1 sec at 16 MHz
}