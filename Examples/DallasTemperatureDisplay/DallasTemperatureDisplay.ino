/*
 * http://code.google.com/p/arduino-seven-segment/
 *
 * This is a basic demo of the seven segment library
 * that displays the current temperature using a Dallas DS18B20 temperature sensor
 *
 * You will need to set your driver and your screen configuration in the line
 * screen.begin("AY0438","8.8|8.8");
 *
 * For best you results you should use a display with 4 digits
 *
 */

/*
 Wiring Info
 ===========

 Requires Dallas DS18B20 temperature sensor on digital pin 2


 ARDUINO Gnd o---------------------o DS18B20 Data pin 1

 ARDUINO +5v o----[ 4.7K resistor ]----o---------o ARDUINO digital pin 2
             |                         |
             |                       DS18B20 Data pin 2
             |
             \---------------------o DS18B20 Data pin 3

*/

#include <stdlib.h>       // Double to long conversion
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SevenSegment.h>

#define CLOCK 44 // Arudino digital 44 -> AY0438 clock
#define DATA 46	 // Arudino digital 46 -> AY0438 data
#define LOAD 45  // Arudino digital 45 -> AY0438 load

SevenSegment screen(CLOCK, DATA, LOAD);

OneWire oneWire(2);
DallasTemperature sensors(&oneWire);
DeviceAddress thermometerAddress;

void setup(){

	Serial.begin(9600);

	// Begin temperature sensors
	sensors.begin();
	sensors.getAddress(thermometerAddress, 0);

	// Set display driver AY0438 and screen definition
	screen.begin("AY0438","8.8|8.8");

}

void loop(){

	// Temperature string
	char temp[8];

	// Retrieve temperature
	sensors.requestTemperatures();

	// Convert temperature to string with 1 decimal place
	dtostrf(sensors.getTempC(thermometerAddress), 0, 1, temp);

	// Print temperature
	screen.print(temp);

	// Wait a bit
	delay(500);
}


