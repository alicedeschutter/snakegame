/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep: A simple function to create a small delay. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
void tick( unsigned int * timep )
{
  /* Get current value, store locally */
  register unsigned int t = * timep;
  t += 1; /* Increment local copy */

  /* If result was not a valid BCD-coded time, adjust now */

  if( (t & 0x0000000f) >= 0x0000000a ) t += 0x00000006;
  if( (t & 0x000000f0) >= 0x00000060 ) t += 0x000000a0;
  /* Seconds are now OK */

  if( (t & 0x00000f00) >= 0x00000a00 ) t += 0x00000600;
  if( (t & 0x0000f000) >= 0x00006000 ) t += 0x0000a000;
  /* Minutes are now OK */

  if( (t & 0x000f0000) >= 0x000a0000 ) t += 0x00060000;
  if( (t & 0x00ff0000) >= 0x00240000 ) t += 0x00dc0000;
  /* Hours are now OK */

  if( (t & 0x0f000000) >= 0x0a000000 ) t += 0x06000000;
  if( (t & 0xf0000000) >= 0xa0000000 ) t = 0;
  /* Days are now OK */

  * timep = t; /* Store new value */
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
  DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);

	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);

	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);

	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
}

void display_image(const uint8_t *data) {
	int i, j;

	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22); // Setup page start and end address
		spi_send_recv(i);

		spi_send_recv(00); //similar to display update function
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 128; j++)
			spi_send_recv(~data[i*128 + j]);
	}}

//from 128x32 to 128x4, sends info to display_image (which displays the screen[32][128] on the display)
void display_full_bin(const uint8_t screen[32][128]){
	int i, j; // i and j are coordinates in 128x4 and i and m are coordinates in 128x32 --> m = jx8
	int p, ind; // index values
	uint8_t array_of_bits[8]; // each of the four bytes are associated with one array containing 8 bits
	uint8_t decimal_value = 0; //uint8_t = signed integer type with width of 8 bits
	uint8_t screen_in_decimal[128*4];

	for(j = 0; j < 4; j++){
			for(i = 0; i  < 128; i ++){
					int m = j * 8; //i and m are coordinates in 128x32 --> m = jx8
					for(p = 0; p < 8; p++){
							if(screen[m][i] == 1){
									array_of_bits[p] = 1; }
							else{
									array_of_bits[p] = 0; }
							m++;}

					for(ind = 7; ind >= 0; ind--){ //transformation of array_of_bits to a decimal value
							decimal_value = array_of_bits[ind] + (decimal_value * 2);}
					screen_in_decimal[i + j*128] = decimal_value; //screen_in_decimal is a list of decimal values, every j coordinate value has 128 i values associated to it
			}}
	display_image((const uint8_t*)screen_in_decimal);} 

void create_screen(int opposite, int x, int y, int width, int height, uint8_t binary_screen[32][128]) {
	int sy, sx;
	int max_width_coord = x + width;
	int max_height_coord = y + height;

	for(sx = x; sx < max_width_coord; sx++) {
		for(sy = y; sy < max_height_coord; sy++) {

			binary_screen[sy][sx] = opposite;} } }

void create_object(int opposite, int x, int y, int width, int height,
	const uint8_t object[], uint8_t binary_screen[32][128]){
	int i, j;
  for(j = 0; j < height; j++){
    for(i = 0; i < width; i++){
			int object_at_coord = object[j*width + i];
			if(opposite){
				if(object_at_coord > 0) {
					binary_screen[y+j][x+i] = 0;}
				else {binary_screen[y+j][x+i] = 1;};}

			else {binary_screen[y+j][x+i] = object_at_coord;};};}}


/* Insert a character (0-9 or A-Z) into a "binary" screen array. Other chars will be interpreted as a blank space.
https://www.cs.cmu.edu/~pattis/15-1XX/common/handouts/ascii.html */
int get_character(int opposite, int x, int y, char c, uint8_t binary_screen[32][128]) {
	if(c >= 48 && c <= 57) { 	// c = "0-9"
		c -= 47;
		create_object(opposite, x, y, 3, 5, number[c], binary_screen);

		return 4;} // 4 = offset to next character position.

	else {
		if(c >= 65 && c <= 90){	// c = "A-Z"
			c -= 64;
			create_object(opposite, x, y, 5, 5, uppercase[c], binary_screen);}

		else if(c == 46) { // .
			create_screen(opposite, x, y + 4, 1, 1, binary_screen);}

		else if(c == 33) { // c = "!"
			create_screen(opposite, x, y, 3, 1, binary_screen);
			create_screen(opposite, x + 4, y, 1, 1, binary_screen);}

		else if(c == 58) { // :
			create_screen(opposite, x, y + 1, 1, 1, binary_screen);
			create_screen(opposite, x, y + 3, 1, 1, binary_screen);}

		else if(c == 63) { // ?
			create_object(opposite, x, y, 5, 5, uppercase[27], binary_screen);} // letter number 27 since index starts at 0

		else{
			create_object(opposite, x, y, 5, 5, uppercase[0], binary_screen);}

		return 6; //6 = offset to next character position.
		}
	}

/* Creates a string of characters (0-9 or A-Z). Places it into screen[32][128].*/
int create_string(int opposite, int x, int y, const char* s, uint8_t binary_screen[32][128]) {
	int offset = 0;
	while(*s != 0) {
		offset += get_character(opposite, x + offset, y, *s, binary_screen);
		s++;
	}
	return offset; //Returns offset to next character position
}

void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;

			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/*
 * itoa
 *
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 *
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 *
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 *
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 *
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 *
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 *
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 *
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";

  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}
