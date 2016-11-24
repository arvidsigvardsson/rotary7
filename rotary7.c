#include <avr/io.h>
#include <util/delay.h>

#define BLINK_DELAY_MS 50

void set7diplay(unsigned char);
char read_input(void);

enum state {
	START,
	INPUT_LOW,
	INPUT_HIGH
};

typedef enum state state_dial;

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

int main (void) {
	// Port D 0-6 output
	DDRD = 0x7f;

	// Pin B4 input
	DDRB &= ~(1 << 4);

	// PB4 låg, ingen pullup
	PORTB &= ~(1 << 4);

	// clear display
	// PORTD = 0; //numbermap[4];

	state_dial current_state = START;
	state_dial next_state = START;

	unsigned char display_number = numbermap[10];
	unsigned char counter = 0;

	uint64_t timer = 0;
	uint64_t timeout = 225000;

	// debug
	DDRB |= (1 << 5);
	// PORTB &= ~(1 << 5);

	// while(!read_input());

	// PORTB |= (1 << 5);
	// while(1);






	while(1) {

		// timer++;
		// if(timer > 225000) {
		// 	PORTB ^= (1 << 5);
		// 	timer = 0;
		// }

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

					_delay_ms(100);
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

		set7diplay(counter);

		// if(display_number > 0) {
		// 	set7diplay(display_number - 1);
		// } else {
		// 	set7diplay(display_number);
		// }
		// set7diplay(display_number);
	}
}

void set7diplay(unsigned char number) {
	PORTD = numbermap[number];
}

char read_input(void) {
	return PINB & (1 << 4);
}