/*
 * DE1SoC_SevenSeg.c
 *
 *  Created on: 12 Feb 2021
 *      Author: Harry Clegg
 *  Updated on: 24 Feb 2024
 *      Author: David Cowell
 *      		You!
 */

#include "DE1SoC_SevenSeg.h"

// ToDo: Add the base addresses of the seven segment display peripherals.
volatile unsigned char *sevenseg_base_lo_ptr = (unsigned char *)0xFF200020;
volatile unsigned char *sevenseg_base_hi_ptr = (unsigned char *)0xFF200030;

// There are four HEX displays attached to the low (first) address.
#define SEVENSEG_N_DISPLAYS_LO 4

// There are two HEX displays attached to the high (second) address.
#define SEVENSEG_N_DISPLAYS_HI 2

void DE1SoC_SevenSeg_Write(unsigned int display, unsigned char value) {
    // Select between the two addresses so that the higher level functions
    // have a seamless interface.
    if (display < SEVENSEG_N_DISPLAYS_LO) {
        // If we are targeting a low address, use byte addressing to access
        // directly.
        sevenseg_base_lo_ptr[display] = value;
    } else {
        // If we are targeting a high address, shift down so byte addressing
        // works.
        display = display - SEVENSEG_N_DISPLAYS_LO;
        sevenseg_base_hi_ptr[display] = value;
    }
}

void DE1SoC_SevenSeg_SetSingle(unsigned int display, unsigned int value) {
    // ToDo: Write the code for driving a single seven segment display here.
    // Your function should turn a real value 0-F into the correctly encoded
    // bits to enable the correct segments on the seven segment display to
    // illuminate. Use the DE1SoC_SevenSeg_Write function you created earlier
    // to set the bits of the display.
	// An array to store the bit-mapping values for each digit
	unsigned char Bitmapping[16] = {
		0x3F, // 0: Segments 0,1,2,3,4,5
		0x06, // 1: Segments 1,2
		0x5B, // 2: Segments 0,1,3,4,6
		0x4F, // 3: Segments 0,1,2,3,6
		0x66, // 4: Segments 1,2,4,6
		0x6D, // 5: Segments 0,2,3,5,6
		0x7D, // 6: Segments 0,2,3,4,5,6
		0x07, // 7: Segments 0,1,2
		0x7F, // 8: Segments 0,1,2,3,4,5,6
		0x6F, // 9: Segments 0,1,2,3,5,6
		0x77, // A: Segments 0,1,2,4,5,6
		0x7C, // B: Segments 2,3,4,5,6
		0x39, // C: Segments 0,3,4,5
		0x5E, // D: Segments 1,2,3,4,5
		0x79, // E: Segments 0,3,4,5,6
		0x71  // F: Segments 0,4,5,6
	};
	// Checking if the value is within the valid range
	unsigned char BitValue = 0;
	if (value < 16) {
		BitValue = Bitmapping[value];
	} else {
		BitValue = 0x40; // Dash symbol if value is out of range
	}

	// Call DE1SoC_SevenSeg_Write with the calculated bit-mapping value
	DE1SoC_SevenSeg_Write(display, BitValue);

}

void DE1SoC_SevenSeg_SetDoubleHex(unsigned int display, unsigned int value) {
    // ToDo: Write the code for setting a pair of seven segment displays
    // here. Good coding practice suggests your solution should call
    // DE1SoC_SevenSeg_SetSingle() twice.
    // This function should show the first digit of a HEXADECIMAL number on
    // the specified 'display', and the second digit on the display to
    // the left of the specified display.

    /** Some examples:
     *
     *    input | output | HEX(N+1) | HEX(N)
     *    ----- | ------ | -------- | ------
     *        5 |     05 |        0 |      5
     *       30 |     1E |        1 |      E
     *     0x60 |     60 |        6 |      0
     */

	if(value < 256){
		DE1SoC_SevenSeg_SetSingle(display, (value& 0x0F)); //Lower nibble
		DE1SoC_SevenSeg_SetSingle(display+1, (value >> 4) & 0x0F); //Higher nibble
	} else {
		DE1SoC_SevenSeg_Write(display, 0x40); // Dash symbol for both displays
		DE1SoC_SevenSeg_Write(display + 1, 0x40);
	}
}

void DE1SoC_SevenSeg_SetDoubleDec(unsigned int display, unsigned int value) {
    // ToDo: Write the code for setting a pair of seven segment displays
    // here. Good coding practice suggests your solution should call
    // DE1SoC_SevenSeg_SetSingle() twice.
    // This function should show the first digit of a DECIMAL number on
    // the specified 'display', and the second digit on the display to
    // the left of the specified display.

    /** Some examples:
     *
     *	  input | output | HEX(N+1) | HEX(N)
     *    ----- | ------ | -------- | ------
     *        5 |     05 |        0 |      5
     *       30 |     30 |        3 |      0
     *     0x60 |     96 |        9 |      6
     */
	if(value <= 99){
		DE1SoC_SevenSeg_SetSingle(display, (value % 10)); //Lower nibble
		DE1SoC_SevenSeg_SetSingle(display+1, (value /10)); //Higher nibble
	} else {
		DE1SoC_SevenSeg_Write(display, 0x40); //Lower nibble
		DE1SoC_SevenSeg_Write(display+1, 0x40); //Higher nibble
	}
}
