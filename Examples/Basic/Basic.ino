/*
 * http://code.google.com/p/arduino-seven-segment/
 *
 * This is a basic demo of the seven segment library
 * that works will work with a single 7 segment display component
 *
 * It will turn on all display segments then turn them off 5 times,
 * count to 9, then display all the characters available.
 *
 * You will need to set your driver and your screen configuration in the line
 * screen.begin("AY0438","8");
 *
 */
#include <SevenSegment.h>

#define CLOCK 44 // Arudino digital 44 -> AY0438 clock
#define DATA 46	 // Arudino digital 46 -> AY0438 data
#define LOAD 45  // Arudino digital 45 -> AY0438 load

SevenSegment screen(CLOCK, DATA, LOAD);

uint16_t counter = 0;

void setup(){

	// Set display driver AY0438 and screen definition
	screen.begin("AY0438","8");

}

void loop(){

	// Turn on all segments and turn off 5 times
	for (int i=0; i<5; i ++){

		// Turn all known segments on
		screen.on();
		delay(500);

		// Turn all known segments off
		screen.clear();
		delay(500);
	}

	// Count to 9
	for (int i = 0; i < 10; i++){
		screen.printNumber(i);
		delay(200);
	}

	// Display all characters one by one
	screen.setAlignment(LEFT);
	char c[2];
	for (int i = 32; i<= 127; i++){
		c[0] = i;
		c[1] = 0;
		screen.print(c);
		delay(200);
	}
}


