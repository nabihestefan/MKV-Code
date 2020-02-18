#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi.h"
// #include "spi.c"

#define CS_DDR DDRC
#define CS_PORT PORTC
#define CS_PIN PC7


// for gTimerFlag
#define UPDATE_STATUS 0

volatile uint8_t gFlag = 0x00;  // Global Flag
volatile uint8_t gTimerFlag = 0x01; // Timer Flag



ISR(TIMER0_COMPA_vect) {
	// Timer Counter0 compare match A
	gTimerFlag |= _BV(UPDATE_STATUS);
}

/*----- Functions -----*/
void initTimer(void) {
	TCCR0A = _BV(WGM01);     // Set up 8-bit timer in CTC mode
	TCCR0B = 0x05;          // clkio/1024 prescaler
	TIMSK0 |= _BV(OCIE0A);
	OCR0A = 0xFF;
}

int main(void) {
	DDRD |= _BV(PD7);
	PORTD |= _BV(PD7);

	CS_DDR |= _BV(CS_PIN);
	CS_PORT |= _BV(CS_PIN);
	uint8_t message = 0x00;
	uint8_t response = 0x00;


	sei();
	initTimer();

	SPI_init(SPI_FOSC_DIV_4, SPI_MODE_0_0 /*Todo: CHECK*/, &CS_PORT, CS_PIN);
	while (1) {
		if(bit_is_set(gTimerFlag,UPDATE_STATUS)) {
			// Check Timer
			gTimerFlag &= ~_BV(UPDATE_STATUS);
			message = 0x00;
			SPI_start();
			for (int i = 0; i < 5; i++) {
				SPI_transfer(message, &response);
				message++;

			}
			SPI_end();
		}
	}
}
