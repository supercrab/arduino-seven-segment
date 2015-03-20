#ifndef SevenSegment_h
#define SevenSegment_h

#define SEVENSEGMENT_LIB_VERSION "1.0"

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

/*
 * Printing alignment
 */
enum alignment_t {
	LEFT,
	RIGHT
};

/*
 * How the digits are wired to the driver
 */
enum wiring_t {
	 LEFT_TO_RIGHT,
	 RIGHT_TO_LEFT
};

/*
 * 7 segment display class
 */
class SevenSegment{
	public:
		SevenSegment(uint8_t clock, uint8_t data);
		SevenSegment(uint8_t clock, uint8_t data, uint8_t load);

		boolean begin(const char* driverName, const char* screenMask);

		void clear();
		void on();
		void display();
		void printError();

		boolean getSegment(uint8_t segment);

		void setWiring(wiring_t wiring);
		void setSegment(uint8_t segment, boolean value);
		void setAlignment(alignment_t alignment);
		void setZeroPadding(boolean padding);
		void setCascaded(boolean cascaded);

		void print(char* s);
		void printAlignedLeft(char* s);
		void printAlignedRight(char* s);

		void printNumber(uint8_t v);
		void printNumber(uint16_t v);
		void printNumber(uint32_t v);

		void printNumber(int8_t v);
		void printNumber(int16_t v);
		void printNumber(int32_t v);

	private:
		uint8_t		_pinClock;
		uint8_t		_pinData;
		uint8_t		_pinLoad;

		boolean		_data[64];
		boolean		_cascaded;
		int8_t		_maskCursor;
		char		_mask[32];
		uint32_t	_overflow;
		int32_t		_underflow;
		alignment_t	_alignment;
		boolean		_zeroPadding;
		wiring_t	_wiring;

		char* trimZeroes(char* pString);

		void putChar(uint8_t c);
		void printChar(uint8_t charToPrint, int8_t direction);
		void clearData();
		void pulseClock();
		void pulseLoad();

		uint8_t getSegmentByte(uint8_t character);
		uint8_t getCharAtMaskCursor();
		uint8_t getMaskCursorIndex();
		uint8_t getMaskCharBitCount(uint8_t maskCharacter);

		boolean calculateDisplayRange();
		boolean canPrintAtMaskCursor(uint8_t charToPrint);
};

#endif
