/*
 * http://code.google.com/p/arduino-seven-segment/
 *
 * This is a demo of the seven segment library
 * that shows how to setting of single segments
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

#define FEATURE_COUNT 2		// Number of times to try each feature
#define DIGIT_COUNT 4 		// Number of digits suuported in screen
#define WAIT 100			// Delay between frames

// Initialise LCD library
SevenSegment screen(CLOCK, DATA, LOAD);


// Index of segment A for each digit in internal data array
int digitLocation[4] = {0, 8, 17, 25};

void setup(){

	// Set display driver AY0438 and screen definition
	screen.begin("AY0438","8.8|8.8");

}

void loop(){

	randomSegments();
	onAndOff();
	spin();
	horizontalBars();
	verticalBars();
	printText();
	scrollingText();
	printAnimation();
	randomSegments();
}

// Turn all segments on and off
void onAndOff(){

	for (int t = 0; t < FEATURE_COUNT; t++){

		// Turn all known segments on
		screen.on();
		delay(WAIT * 5);

		// Turn all known segments off
		screen.clear();
		delay(WAIT * 5);
	}
}

// Do spin animation
void spin(){

	for (int t = 0; t < FEATURE_COUNT; t++){
		for (int s = 0; s < 6; s++){

			// Turn on specific segments
			for (int d = 0; d < DIGIT_COUNT; d++){
				screen.setSegment(digitLocation[d] + s, true);
			}

			// Send display information to screen & wait
			screen.display();
			delay(WAIT);

			// Turn the segments back off internally
			for (int d = 0; d < DIGIT_COUNT; d++){
				screen.setSegment(digitLocation[d] + s, false);
			}
		}
	}
}

// Animate horizontal segments
void horizontalBars(){

	// Segment array for horizontal bars
	int horizontalBarsData[3] = {3, 6, 0};

	for (int t = 0; t < FEATURE_COUNT; t++){

		// Draw bars
		for (int i = 0; i < 3; i++){
			for (int d = 0; d < DIGIT_COUNT; d++){
				screen.setSegment(digitLocation[d] + horizontalBarsData[i], true);

			}
			screen.display();
			delay(WAIT);
		}

		// Clear bars
		for (int i = 0; i < 3; i++){
			for (int d = 0; d < DIGIT_COUNT; d++){
				screen.setSegment(digitLocation[d] + horizontalBarsData[i], false);

			}
			screen.display();
			delay(WAIT);
		}
	}
}

// Animate vertical segments
void verticalBars(){

	for (int t = 0; t < FEATURE_COUNT; t++){

		// Draw bars
		for (int d = 0; d < DIGIT_COUNT; d++){
			screen.setSegment(digitLocation[d] + 1, true);
			screen.setSegment(digitLocation[d] + 2, true);
			screen.display();
			delay(WAIT);
			screen.setSegment(digitLocation[d] + 4, true);
			screen.setSegment(digitLocation[d] + 5, true);
			screen.display();
			delay(WAIT);
		}


		// Clear bars
		for (int d = 0; d < DIGIT_COUNT; d++){
			screen.setSegment(digitLocation[d] + 1, false);
			screen.setSegment(digitLocation[d] + 2, false);
			screen.display();
			delay(WAIT);
			screen.setSegment(digitLocation[d] + 4, false);
			screen.setSegment(digitLocation[d] + 5, false);
			screen.display();
			delay(WAIT);
		}
	}
}

// Print some text
void printText(){

	char* message[6] = { "This", "is", "Some", "Cool", "LCD", "SHIT" };

	for (int t = 0; t < FEATURE_COUNT; t++){
		for (int m = 0; m < 6; m++){
			screen.print(message[m]);
			delay(WAIT * 5);
		}
	}
	screen.clear();
}

// Scroll some text
void scrollingText(){

	char buf[] = "    Sometimes you have to scroll to get the message across... ";

	for (char* p = buf; p < buf + strlen(buf); p++){
		screen.print(p);
		delay(WAIT * 2);
	}

	screen.clear();
}

// Animate some zeroes
void printAnimation(){

	char* message[9] = { ".", "o", "0", "()", "( )", "(  )", "POP", "----", "" };

	for (int t = 0; t < FEATURE_COUNT; t++){
		for (int m = 0; m < 9; m++){
			screen.print(message[m]);
			delay(WAIT * 5);
		}
	}
	screen.clear();
}

// Toggle random segments on and off
void randomSegments(){
	for (int t = 0; t < 100; t++){
		int segment = random(32);
		screen.setSegment(segment, !screen.getSegment(segment));
		screen.display();
		delay(WAIT);
	}
	screen.clear();
}