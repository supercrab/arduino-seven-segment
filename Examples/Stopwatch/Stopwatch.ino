/*
 * http://code.google.com/p/arduino-seven-segment/
 *
 * This is a demo of the seven segment library
 * that displays a basic stop watch
 *
 * You will need to set your driver and your screen configuration in the line
 * screen.begin("AY0438","8.8|8.8");
 *
 * For best you results you should use a display with 4 digits
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
	screen.begin("AY0438","8.8|8.8");

}

void loop(){

	// String to print
	static char buf[8];

	// Convert count to string
	sprintf(buf, "%04d\0", counter);

	// Insert colon into string
	buf[5] = buf[4];
	buf[4] = buf[3];
	buf[3] = buf[2];
	buf[2] = ':';

	// Display
	screen.print(buf);

	// Increment counter
	counter++;

	// Wait a bit
	delay(100);
}


