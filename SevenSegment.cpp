/*
 * LCD/LED library for 7 segment displays
 * Date: Jan 2012
 * Author: M Mason
 * Website: http://code.google.com/p/arduino-seven-segment/
 *
 * This is a library for controlling 7 segment LCD or LED displays using
 * any of the following display drivers
 *
 * M5450  (LED, 34 outputs)
 * M5451  (LED, 35 outputs)
 * MM5452 (LCD, 32 outputs)
 * MM5453 (LCD, 33 outputs)
 * AY0438 (LCD, 32 outputs)
 * BT-M512RD-DR1 (LED, 35 outputs)
 *
 * See the examples for usage
 */
#include "SevenSegment.h"
#include <avr/pgmspace.h> // Flash memory functions

/*
 * Character segment definitions are in the format GFEDCBA
 *
 *     __A__
 *    |     |
 *   F|     |B
 *    |__G__|
 *    |     |
 *   E|     |C
 *    |__D__|
 *
 */

 /*

 Timing Diagram for AY0438
 =========================
                                   :
                      ____        _:__        ____        ____
                     / 1  \      / :  \      / 31 \      / 32 \
   CLOCK ___________/      \____/  :   \____/      \____/      \______________
               start               :
         ____________  __________  :_________  __________  __________  _______
                     \/  SEG 32  \/:         \/  SEG 2   \/  SEG 1   \/
 Data in ____________/\__________/\:_________/\__________/\__________/\_______
                                   :
                                   :                               ____
                                   :                              |    |
    Load __________________________:______________________________|    |______
                                   :

*/

PROGMEM const prog_uchar segmentMap[] = {
	B0000000, // 032 SPACE
	B0110000, // 033 !
	B0100010, // 034 "
	B1000001, // 035 #
	B1101101, // 036 $
	B1010010, // 037 %
	B1111100, // 038 &
	B0100000, // 039 '
	B0111001, // 040 (
	B0001111, // 041 )
	B0100001, // 042 *
	B1110000, // 043 +
	B0001000, // 044 ,
	B1000000, // 045 -
	B0001000, // 046 .
	B1010010, // 047 /
	B0111111, // 048 0
	B0000110, // 049 1
	B1011011, // 050 2
	B1001111, // 051 3
	B1100110, // 052 4
	B1101101, // 053 5
	B1111101, // 054 6
	B0000111, // 055 7
	B1111111, // 056 8
	B1101111, // 057 9
	B1001000, // 058 :
	B1001000, // 059 ;
	B0111001, // 060 <
	B1001000, // 061 =
	B0001111, // 062 >
	B1010011, // 063 ?
	B1011111, // 064 @
	B1110111, // 065 A
	B1111100, // 066 B
	B0111001, // 067 C
	B1011110, // 068 D
	B1111001, // 069 E
	B1110001, // 070 F
	B0111101, // 071 G
	B1110110, // 072 H
	B0000110, // 073 I
	B0011110, // 074 J
	B1110110, // 075 K
	B0111000, // 076 L
	B0010101, // 077 M
	B0110111, // 078 N
	B0111111, // 079 O
	B1110011, // 080 P
	B1100111, // 081 Q
	B0110001, // 082 R
	B1101101, // 083 S
	B1111000, // 084 T
	B0111110, // 085 U
	B0011100, // 086 V
	B0101010, // 087 W
	B1110110, // 088 X
	B1101110, // 089 Y
	B1011011, // 090 Z
	B0111001, // 091 [
	B1100100, // 092 BACKSLASH (backslash character in code means line continuation)
	B0001111, // 093 ]
	B0100011, // 094 ^
	B0001000, // 095 _
	B0100000, // 096 `
	B1110111, // 097 a
	B1111100, // 098 b
	B1011000, // 099 c
	B1011110, // 100 d
	B1111001, // 101 e
	B1110001, // 102 f
	B1101111, // 103 g
	B1110100, // 104 h
	B0000100, // 105 i
	B0011110, // 106 j
	B1110110, // 107 k
	B0011000, // 108 l
	B0010101, // 109 m
	B1010100, // 110 n
	B1011100, // 111 o
	B1110011, // 112 p
	B1100111, // 113 q
	B1010000, // 114 r
	B1101101, // 115 s
	B1111000, // 116 t
	B0111110, // 117 u
	B0011100, // 118 v
	B0101010, // 119 w
	B1110110, // 120 x
	B1101110, // 121 y
	B1011011, // 122 z
	B0111001, // 123 {
	B0110000, // 124 |
	B0001111, // 125 }
	B1000000, // 126 ~
	B0000000, // 127 DEL
	B1100011  // 128 Degree symbol
};


/*
 * LCD/LED Driver structure
 */
struct driver {
	char* name;
	uint8_t segments;
	boolean pulseLoad;
	boolean segmentsOrderInc;
	boolean initialBit;
	boolean cascadable;
	boolean dataEnable;
}
_device;

/*
 * Table of supported drivers stored in program memory
 */
PROGMEM const driver driverTable[] = {
	{ "M5450", 34, false, true, true, false, false },
	{ "M5451", 35, false, true, true, false, false },
	{ "MM5452", 32, false, true, true, false, true },
	{ "MM5453", 33, false, true, true, false, false },
	{ "AY0438", 32, true, false, false, true, false },
	{ "BT-M512RD-DR1", 35, false, true, true, false, true }
};

// Driver table reference
driver* pDriverTable = (driver*) &driverTable;


#define NUMBER_LENGTH 4  // Number of digits in a number
#define DRIVER_COUNT (sizeof(driverTable)/sizeof(driver)) // Number of driver chips supported
#define MASK_COUNT 32 // Number of digits, symbols supported by screen mask

#define SERIAL_MESSAGES_ON 0  // Debug messages On/off
#define SERIAL_BAUD 9600  // Serial baud rate

#define FIRST_CHARACTER 32 // ASCII value of first character defined in segment mapping array
#define LAST_CHARACTER 128 // ASCII value of last character defined in segment mapping array

#include "SevenSegment.h"

/*
 * Display constructor
 */
SevenSegment::SevenSegment(uint8_t clock, uint8_t data) {

	_pinClock = clock;
	_pinData = data;

	pinMode(_pinClock, OUTPUT);
	pinMode(_pinData, OUTPUT);
}

/*
 * Display constructor
 */
SevenSegment::SevenSegment(uint8_t clock, uint8_t data, uint8_t load) {

	_pinClock = clock;
	_pinData = data;
	_pinLoad = load;

	pinMode(_pinClock, OUTPUT);
	pinMode(_pinData, OUTPUT);
	pinMode(_pinLoad, OUTPUT);
}

/*
 * Initialise LCD using specified driver chip and with specified screen configuartion
 */
boolean SevenSegment::begin(const char* driverName, const char* screenMask ){

	// Temp buffer
	char buffer[12];

	// Loop through driver table and find relevant driver
	for (uint8_t i=0; i < DRIVER_COUNT; i++){

		// Copy driver name to temp buffer
		strcpy(buffer, (char*) pgm_read_word(&(pDriverTable[i].name)));

		// Is this driver the one specified?
		if (strcmp(buffer, driverName)==0){

			// Copy driver data into the struct in main memory
			_device.name = (char*) pgm_read_word(&(pDriverTable[i].name));
			_device.segments = (uint8_t) pgm_read_word(&(pDriverTable[i].segments));
			_device.pulseLoad = (boolean) pgm_read_word(&(pDriverTable[i].pulseLoad));
			_device.segmentsOrderInc = (boolean) pgm_read_word(&(pDriverTable[i].segmentsOrderInc));
			_device.initialBit = (boolean) pgm_read_word(&(pDriverTable[i].initialBit));
			_device.cascadable = (boolean) pgm_read_word(&(pDriverTable[i].cascadable));

			// Store screen mask
			strcpy(_mask, screenMask);

			// Calculate over/underflow values
			boolean maskValid = calculateDisplayRange();

#if SERIAL_MESSAGES_ON > 0
			Serial.print("Driver found: ");
			Serial.println(_device.name);
			if (maskValid){
				Serial.print("Maximum value supported: ");
				Serial.println(_overflow);
				Serial.print("Minimum value supported: ");
				Serial.println(_underflow);
			}
			else {
				Serial.println("Mask invalid!");
			}
#endif
			// If mask is valid then setup defaults
			if (maskValid){

				_wiring = LEFT_TO_RIGHT;
				_maskCursor = 0;
				_alignment = LEFT;
				_zeroPadding = false;
				clear();

				return true;
			}
			else{
				return false;
			}
		}
	}

#if SERIAL_MESSAGES_ON > 0
	Serial.print("Could not find display driver!");
#endif

	// Could not find the specified driver :(
	return false;
}


/*
 * Print unsigned numbers
 */
void SevenSegment::printNumber(uint8_t v){
	printNumber((uint32_t) v);
}

void SevenSegment::printNumber(uint16_t v){
	printNumber((uint32_t) v);
}

void SevenSegment::printNumber(uint32_t v){

	uint32_t r;
	char buf[NUMBER_LENGTH + 2];

	// Ensure that array is clear!
	memset(buf, 0, sizeof buf);

	// Does number fit on screen?
	if (v > _overflow){
		printError();
		return;
	}

	// Convert number to string
	for (uint8_t x = NUMBER_LENGTH; x-- > 0; ){
		r = v % 10;
		v /= 10;
		buf[x] = '0' + r;
	}

	// Strip leading zeroes
	if (_alignment == LEFT || (_alignment == RIGHT && !_zeroPadding)){

		trimZeroes(buf);

		// If we've trimmed all zeros and there's nothing left put a zero back in!
		if (strlen(buf) == 0){
			buf[0] = '0';
			buf[1] = 0;
		}
	}

	// Display
	print(buf);
}


/*
 * Print signed numbers
 */
void SevenSegment::printNumber(int8_t v){
	printNumber((int32_t) v);
}

void SevenSegment::printNumber(int16_t v){
	printNumber((int32_t) v);
}

void SevenSegment::printNumber(int32_t v){

	char buf[NUMBER_LENGTH + 2];
	boolean negative = false;

	// Ensure that array is clear!
	memset(buf, 0, sizeof buf);

	uint8_t r = 0;
	uint8_t c = 0;

	// Does number fit on screen?
	if (v < _underflow || (v > 0 && (uint32_t) v >  _overflow)) {
		printError();
		return;
	}

	// Setup for negative display
	if (v < 0) {
		buf[0] = '0';
		c = 1;
		v = -v;
		negative = true;
	}

	// Put digits into char array
	for (uint8_t x = NUMBER_LENGTH; x-- > c; ) {
		r = v % 10;
		v /= 10;
		buf[x] = '0' + r;
	}

	// Strip leading zeroes?
	if (_alignment == LEFT || (_alignment == RIGHT && !_zeroPadding)){

		trimZeroes(buf);

		// If we've trimmed all zeros and there's nothing left put a zero back in!
		if (strlen(buf) == 0){
			buf[0] = '0';
			buf[1] = 0;
		}

		// If number is negative move characters to the right so we can insert minus later
		if (negative){
			for (int8_t c = strlen(buf); c >= 0; c--){
				buf[c + 1] = buf[c];
			}
		}
	}

	// Put minus back in
	if (negative){
		buf[0] = '-';
	}

	// Display
	print(buf);

}

/*
 * Trim spaces from start of a string
 */
char* SevenSegment::trimZeroes(char* pString){
	int nLen = strlen(pString);
	size_t nPos = strspn(pString, "0");
	if ( nPos > nLen || nPos == 0 )
	return pString;
	memmove( pString, pString + nPos, nLen - nPos);
	pString[nLen-nPos] = 0;
	return pString;
}


/*
 * Print string pointer
 */
void SevenSegment::print(char* s){

	if (_alignment == LEFT){
		printAlignedLeft(s);
	}
	else{
		printAlignedRight(s);
	}
}


/*
 * Print string pointer aligned to the left
 */
void SevenSegment::printAlignedLeft(char* s){
	clearData();
	_maskCursor = 0;
	while (*s != 0 && _maskCursor < strlen(_mask)) {
		printChar(*s, 1);
		s++;
	}
	display();
}

/*
 * Print string pointer aligned right
 */
void SevenSegment::printAlignedRight(char* s){

	clearData();
	_maskCursor = strlen(_mask) - 1;
	for (char* p = s + strlen(s)-1; p >= s; p--){
		printChar(*p, -1);
	}
	display();
}

/*
 * Clear screen
 */
void SevenSegment::clear(){
	clearData();
	display();
}

/*
 * Turn all segments on
 */
void SevenSegment::on(){
	memset(_data, 1, sizeof _data);
	display();
}

/*
 * Clear all display data
 */
void SevenSegment::clearData(){
	memset(_data, 0, sizeof _data);
}

/*
 * Get status of single segment
 */
boolean SevenSegment::getSegment(uint8_t segment){
	return _data[segment];
}

/*
 * Set single segment
 */
void SevenSegment::setSegment(uint8_t segment, boolean value){
	_data[segment] = value;
}

/*
 * Set the order the digits are arranged
 */
void SevenSegment::setWiring(wiring_t wiring){
	_wiring = wiring;
}

/*
 * Set the order the digits are arranged
 */
void SevenSegment::setAlignment(alignment_t alignment){
	_alignment = alignment;
}

/*
 * Set whether to pad numbers with 0s when printing right aligned
 */
void SevenSegment::setZeroPadding(boolean padding){
	_zeroPadding = padding;
}

/*
 * Set cascaded - only supported on AY0438s which doubles segment count to 64
 */
void SevenSegment::setCascaded(boolean cascaded){
	if (_device.cascadable){
		_cascaded = cascaded;
	}
}

/*
 * Get the segments defined for a character as a byte
 */
uint8_t SevenSegment::getSegmentByte(uint8_t character){

	// Out of range so display a blank
	if (character < FIRST_CHARACTER || character > LAST_CHARACTER){
		return 0;
	}
	else {
		// read character from progmem
		return pgm_read_byte(&segmentMap[character - FIRST_CHARACTER]);
	}
}

/*
 * Calculate the maximum and minimum values that can be displayed
 */
boolean SevenSegment::calculateDisplayRange(){

	uint8_t fullDigits = 0;
	boolean foundDigit = false;
	boolean foundMinus = false;
	boolean foundOne = false;

	// Clear previous values just in case
	_overflow = 0;
	_underflow = 0;

	// Examine each character in the screen configuration
	for (uint8_t c = 0; c < strlen(_mask); c++){

		switch (_mask[c]){

		// A minus character should appear once and before anything else
		case '-':
			if (!foundMinus && !foundOne && !foundDigit)
				foundMinus = true;
			else
				return false;
			break;

		// A single 1 digit should only appear once before a digit
		case '1':
			if (!foundOne && !foundDigit){
				foundOne = true;
				_overflow = 1;
			}
			else
				return false;
			break;

		// 8 any digit can be displayed at this position
		case '8':
			foundDigit = true;
			fullDigits++;
			_overflow *= 10;
			_overflow += 9;
			if (fullDigits > 1){
				_underflow *= 10;
				_underflow -= 9;
			}
			break;

		// Decimal points, colons and pipes can not appear more than once consecutively
		case '.':
		case '|':
		case ':':
			if (c > 0 && (_mask[c-1] == '.' || _mask[c-1] == '|' || _mask[c-1] == ':'))
				return false;
			break;

		// Invalid character in mask :(
		default:
			return false;
		}
	}

	// If mask starts with a minus then underflow is always overflow * -1
	if (foundMinus)
		_underflow = _overflow * -1;

	// All OK
	return true;
}

// ---------------------------------------------
// Data pulsing
// ---------------------------------------------

/*
 * Pulse clock
 */
void SevenSegment::pulseClock(){
	digitalWrite(_pinClock, HIGH);
	digitalWrite(_pinClock, LOW);
}

/*
 * Pulse load
 */
void SevenSegment::pulseLoad(){
	digitalWrite(_pinLoad, HIGH);
	digitalWrite(_pinLoad, LOW);
}

/*
 * Outputs data to screen :)
 */
void SevenSegment::display(){

	uint8_t segmentCount = _device.segments;

	// If driver is cascaded then doubles the total segment count
	if (_cascaded){
		segmentCount *= 2;
	}

	// Set data enable to low
	if (_device.dataEnable){
		digitalWrite(_pinLoad, LOW);
	}

	// Send initial bit if required
	if (_device.initialBit){
		digitalWrite(_pinData, HIGH);
		pulseClock();
	}

	// Write data segments
	if (_device.segmentsOrderInc){

		// Output data in ascending order
		for (uint8_t i = 0; i < segmentCount; i++){
			digitalWrite(_pinData, _data[i] ? HIGH : LOW);
			pulseClock();
		}
	}
	else{

		// Output data in descending order
		for (int8_t i = segmentCount - 1; i >= 0; i--){
			digitalWrite(_pinData, _data[i] ? HIGH : LOW);
			pulseClock();
		}
	}

	// Pulse load if required
	if (_device.pulseLoad){
		pulseLoad();
	}

	// Set data enable to high
	if (_device.dataEnable){
		digitalWrite(_pinLoad, HIGH);
	}
}

/*
 * Returns the mask character at current mask cursor position
 */
uint8_t SevenSegment::getCharAtMaskCursor(){
	return _mask[_maskCursor];
}

/*
 * Put character data into display array
 */
void SevenSegment::printChar(uint8_t charToPrint, int8_t direction){

	uint8_t maskChar = getCharAtMaskCursor();
// charToPrint == '.' ||
	if (charToPrint == ':' || charToPrint == '|'){

		// display punctuation if printable at current location
		if (canPrintAtMaskCursor(charToPrint)){
			putChar(charToPrint);
			_maskCursor += direction;
		}
	}
	else if (charToPrint == ' '){

		// automatically clear any punctuation at current location
		if (maskChar == '.'  || maskChar == ':'  || maskChar == '|'){
			putChar(' ');
			_maskCursor += direction;
		}
		putChar(' ');
		_maskCursor += direction;
	}
	else {

		// try to output character at the next valid position
		while ((direction == 1 && _maskCursor < strlen(_mask)) || (direction == -1 && _maskCursor >= 0)){

			if (canPrintAtMaskCursor(charToPrint)){
				putChar(charToPrint);
				_maskCursor += direction;
				break;
			}
			_maskCursor += direction;
		}
	}
}

/*
 * Can we output specified character at current mask position
 */
boolean SevenSegment::canPrintAtMaskCursor(uint8_t charToPrint){

	uint8_t maskChar = getCharAtMaskCursor();

	if (maskChar == '8'){
		if (charToPrint != '.' && charToPrint != ':' && charToPrint != '|'){
			return true;
		}
	}
	else if (maskChar == '|'){
		if (charToPrint == ':' || charToPrint == '.' || charToPrint == '|' || charToPrint == ' '){
			return true;
		}
	}
	else if (maskChar == '.' ||  maskChar == ':' || maskChar == '-' || maskChar == '1' ){
		if (charToPrint == maskChar || charToPrint == ' '){
			return true;
		}
	}

	return false;
}

/*
 * Fill display with minus symbols
 */
void SevenSegment::printError(){

	_maskCursor = 0;
	for (uint8_t c = 0; c < strlen(_mask); c++){
		if (_mask[c] == '8' || _mask[c] == '-'){
			putChar('-');
		}
		_maskCursor++;
	}
	display();
}

/*
 * Put a character into screen data at current position
 */
void SevenSegment::putChar(uint8_t c){

	uint8_t maskChar = getCharAtMaskCursor();
	uint8_t index = getMaskCursorIndex();
	uint8_t dataBits;
	uint8_t bitCount = getMaskCharBitCount(maskChar);

	// What data we need to store and pack into correct number of bits
	switch (maskChar){
	case '8':
		dataBits = getSegmentByte(c);
		break;
	case '|':
		if (c == '.')
			dataBits = B00000001;
		else if (c == ':')
			dataBits = B00000010;
		else if (c == '|')
			dataBits = B00000011;
		else
			dataBits = 0;
		break;
	case '.':
	case ':':
	case '-':
	case '1':
		if (c != ' ')
			dataBits = 1;
		else
			dataBits = 0;
		break;
	}

#if SERIAL_MESSAGES_ON > 0
	Serial.print("Char to print is ");
	Serial.print(c, BYTE);Serial.print("(");Serial.print(c, DEC);Serial.print(")");
	Serial.print(" at index ");
	Serial.print(index, DEC);
	Serial.print(".  Maskchar is ");
	Serial.println(maskChar);
#endif

	// Put data into our segment array
	for (uint8_t i = 0; i < bitCount; i++) {
		_data[index + i] = dataBits & (1 << i) ? true : false;
	}
}


/*
 * Returns where our cursor position is.  0 is the first character on the left
 */
uint8_t SevenSegment::getMaskCursorIndex(){

	uint8_t index = 0;

	// Calculate mask element position in array
	if (_wiring == LEFT_TO_RIGHT) {
		for (uint8_t c = 0; c < _maskCursor; c++) {
			index += getMaskCharBitCount(_mask[c]);
		}
	}
	else {
		for (uint8_t c = strlen(_mask) - 1; c > _maskCursor; c--) {
			index += getMaskCharBitCount(_mask[c]);
		}
	}
	return index;
}

/*
 * Returns the number of segments/bits used to store specified character in data array
 */
uint8_t SevenSegment::getMaskCharBitCount(uint8_t maskCharacter){

	uint8_t bitCount = 0;

	switch (maskCharacter){
	case '8':
		bitCount = 7;  // 7 bits for a digit
		break;
	case '|':
		bitCount = 2; // 2 bits for a colon or point
		break;
	case '.':
	case ':':
	case '-':
	case '1':
		bitCount = 1; // 1 bit for colon, point, minus or 1
		break;
	}
	return bitCount;
}

