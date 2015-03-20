Arduino Library for 7 Segment LCD/LED Displays
==============================================

Welcome to the Arduino 7 Segment display library which provides easy control of 7 segment LCD and LED displays using a minimum of 2 digital outputs!  The library only works with parallel displays, where each segment on the display has a single corresponding pin to control it.  For example, if your screen can display 8888 but has less than 7 + 7 + 7 + 7 = 28 pins on the back, then this library is probably not for you.  Additionally, if your display has a serial or SPI interface then this is also not for you.

Here's a youtube video of the driver running the 4 Digit Demo program:

http://www.youtube.com/watch?v=nGXTJL51D6Q

This library can also be used to control LEDs which are not part of a 7 segment display.  However, if you want to do this, there is a much better library that implements a variety of features, including PWM for LED brightness, visit http://code.google.com/p/arduino-m5451-current-driver/

In order to control many LCD/LED segments using an Arduino (a typical 4 digit display has 32 segments) a display driver is required.  These are integrated circuits which receive a serial input and only require a clock source, data, +5v and ground, with the AY0438 LCD driver requiring an additional Load input. 

The supported display drivers are very basic in operation and simply remember the status of a data pin (high or low) every time the clock pin changes from high to low.  The correct choice of driver will depend on whether you have an LED or LCD screen and how many outputs you require.  It is also worth noting that you may cascade 2 AY0438 chips to give you control of up 64 LCD segments!  The following display drivers are currently supported:
 
M5450  (LED, 34 outputs)
M5451  (LED, 35 outputs)
MM5452 (LCD, 32 outputs)
MM5453 (LCD, 33 outputs)
AY0438 (LCD, 32 outputs)

The AY0438 LCD driver can drive a standard 4 digit LCD screen perfectly.  A typical 4 digit LCD display should be able to display 8.8.:8.8 - a digit, decimal point, a digit, decimal point and/or colon, digit, decimal point and a digit.

It may be possible for the other display drivers to duplex or multiplex but this is currently not implemented.  

Setup
-----

You should consult the datasheet for your display driver first (see datasheets folder) and I have included some brief instructions on how to wire everything up.  The drivers are very simple to use and are all quite similar in operation.  

There is some extra help wiring up M5451 and M5450 if you need it here  http://code.google.com/p/arduino-m5451-current-driver/wiki/ArduinoM5451

Connect your Arduino up to the display driver as shown, you can use any digital pin, but since pins 0, 1, and 2 are used for interupts, I recommend using pins 3, 4 and 5, if required.

 ARDUINO       DRIVER PIN
 
 +5v       --> Vdd 
 Ground    --> Vss
 Digital 3 --> Clock
 Digital 4 --> Data

If you have the AY0438 driver you also need to connect a digital pin to Load on the driver:
 
 Digital 5 --> Load (AY0438 only)

You will now need to connect your driver to your display.  This is time consuming so you should find a datasheet for your device to save you time when wiring it up.  It is important that you wire the digit segments in order from A to G.  If your LCD/LED screen has a decimal point and colon in the same position (if 08:50 and 08.50 is possible) wire the decimal point first then the colon.

 DRIVER     SCREEN

 Seg 01 --> Digit 1, segment A
 Seg 02 --> Digit 1, segment B
 Seg 03 --> Digit 1, segment C
 Seg 04 --> Digit 1, segment D
 Seg 05 --> Digit 1, segment E
 Seg 06 --> Digit 1, segment F
 Seg 07 --> Digit 1, segment G
 Seg 08 --> Decimal point 1
 Seg 09 --> Digit 2, segment A
 Seg 10 --> Digit 2, segment B
 Seg 11 --> Digit 2, segment C
 Seg 12 --> Digit 2, segment D
 Seg 13 --> Digit 2, segment E
 Seg 14 --> Digit 2, segment F
 Seg 15 --> Digit 2, segment G
 Seg 16 --> Decimal point 2
 Seg 17 --> Colon
 Seg 18 --> Digit 3, segment A
 :          :
 Seg 32     Digit 4, segment G

NOTE:

* If you are using LEDs they should all have a common anode: all positive ends of the LEDs should be wired to +5V and the negatives to the driver so that current flows into the driver.

* If you have an LCD screen you will also need to connect the backplane and driver oscillators as below.  The backplanes could be controlled using the arduino but I decided to keep thigs simple and avoid this.
 
AY0438
------

BP    --> LCD Backplane
LCD   --> Capacitor --> Arduino Ground   

The capacitor value can vary from 20 pF for 140 hz - 120pF for 50 Hz display refresh.  This can be found in the datasheet.

MM5452/MM5453
-------------

BP OUT --> LCD Backplane
BP IN  --> LCD Backplane
OSC IN --> 0.01 uF Capacitor --> Arduino Ground
OSC IN --> 50K --> Driver VDD


Coding
------

The driver is initilised using begin() which tells the library which display driver is used and what the screen definition is.  You can use any of the following characters to tell the library how your screen is wired to your driver:

- minus
. decimal point
: colon
| decimal point and colon
8 digit

Examples:

screen.begin("M5451","8");	// LED driver with 1 digit screen
screen.begin("M5451","8.8");	// LED driver with 2 digit screen with decimal points
screen.begin("M5451","8888");	// LED driver with 4 digit screen
screen.begin("AY0438","-1888");	// LCD driver, screen wired: minus, one, 3 digits
screen.begin("AY0438","8.8.8.8");// LCD driver, digit, point, digit, point, digit, point, digit
screen.begin("AY0438","8.8|8.8");// LCD driver, digit, point, digit, point and colon, digit, point, digit
screen.begin("AY0438","88:88:88"); // LCD driver, stopwatch style. This would require 2 cascade AY0438s

Printing of punctuation
-----------------------

* When priting decimal points and colons to the screen they will only appear if they have been defined in the screen definition at the current cursor position.  
* If you define your screen as "88" then print("8.8") or print(".8.8.") will always display as "88" on the screen.
* If your screen definition is "8.8" and you print("88") the decimal point is automatically cleared.  
* If you define you screen as "8.8|8.8" you may use the | in the print method to display the point point and colon simultaneously, i.e. print("88|88").

LCD vs LED
----------

LCD screens differ from their LED counterparts in that they require an alternating current.  Supplying a direct current to an LCD screen will eventually cause electrochemical action which degrades the display, resulting in a loss of alignment along the edges of some of the characters.  When using the LCD drivers you should always use a capacitor which briefly supplies the reverse current used by the driver chip.  The driver chip does the rest and will oscillate the current.

LCD is great for low power applications.

Credits
-------

The SevenSegment code has been writted by M Mason and developed using an AY0438 driver and 4 digit LCD-S401C39TR Lumex LCD numeric display module.

Please send any comments, suggestions or feedback to splendidcrab[at]gmail.com.

Website
-------

You can find the latest version of the library at 

http://code.google.com/p/arduino-seven-segment

License
-------

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
