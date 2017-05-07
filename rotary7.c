/*
 *
 * A program for digitizing input from a rotary phone, and display the result on a 7 segment display, for the Arduino Uno board. A rotary disc works by creating a number of electrical pulses corresponding to the entered digit.
 * Author: Arvid Sigvardsson
 *
 */

#include <avr/io.h>
#include <util/delay.h>

#define BLINK_DELAY_MS 50

void set_7_display(unsigned char);
char read_input(void);
void set_display_position(uint8_t);

enum state {
	START,
	INPUT_LOW,
	INPUT_HIGH
};

typedef enum state state_dial;

// hexadecimal codes where index 0 corresponds to digit 0 on a 7 segment display. The segments A to G on the display should be connected to PORTDs pins 0-6
char numbermap[] = {	0x00,
			0x3f,
			0x06,
			0x5b,
			0x4f,
			0x66,
			0x6d,
			0x7d,
			0x07,
			0x7f,
			0x6f,
			0x00};

// outputs a digit to the 7 segment display, if the segments A to G are connected to PORTDs pins 0-6
void set_7_display(unsigned char number) {
	PORTD = numbermap[number];
}

// reads state of input pin
char read_input(void) {
	return PINB & (1 << 4);
}

// to enable multiple digits displayed, which I have not implemented
void set_display_position(uint8_t pos) {
	PORTB = 0x00 | (1 << pos);
}

int main (void) {
	// Port D 0-6 output
	DDRD = 0x7f;

	// Pin B4 input
	// DDRB &= ~(1 << 4);

	// PB 0-3 outputs, PB 4 input
	DDRB = 0x0f;

	// PB4 low, no pullup
	PORTB &= ~(1 << 4);

	state_dial current_state = START;
	state_dial next_state = START;

	unsigned char display_number = numbermap[10];
	unsigned char counter = 0;

	uint64_t timer = 0;
	uint64_t timeout = 225000;

	int posidx = 0;
 	while(1) {

		// finite state machine for main operation
		switch(current_state) {
			case START:
				timer = 0;
				if(read_input()) {
					next_state = INPUT_HIGH;
					counter = 0;
					display_number = 0;
				}
				break;

			case INPUT_HIGH:
				timer++;
				if(read_input()) {
					next_state = INPUT_LOW;
					counter++;

					_delay_ms(75);
				}
				if(timer > timeout) {
					next_state = START;
					PORTB ^= (1 << 5);
 				}
 				display_number = counter;
				break;

			case INPUT_LOW:
				timer++;

				if(!read_input()) {
					next_state = INPUT_HIGH;
					//_delay_ms(100);
				}
				if(timer > timeout) {
						next_state = START;
						PORTB ^= (1 << 5);

 				}
 				display_number = counter;
				break;
		}

		current_state = next_state;

		// outputs entered number. Counts up, so if 2 is entered, first 0, then 1, then 2 is displayed
		set_7_display(counter);
	}
}
