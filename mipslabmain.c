#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

uint8_t screen[32][128];

int main(void) {
        /*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */

	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;

	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;

	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);

	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
  SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;

	display_init();

	create_screen(1, 0, 0, 128, 32, screen);
  create_string(0, 64 - (sizeof("SNAKE GAME")*6) / 2, 1, "SNAKE GAME", screen);
	display_full_bin(screen);
	quicksleep(10000000);

  create_string(0, 64 - (sizeof("ARE YOU READY")*6) / 2, 9, "ARE YOU READY", screen);
  create_string(0, 64 - (sizeof("TO PLAY?")*6) / 2, 15, "TO PLAY?", screen);
	display_full_bin(screen);
	quicksleep(10000000);

	labinit();
	while (1) {

		int switches = getsw();

		create_screen(0, 0, 0, 128, 32, screen);
	  create_string(1, 64 - (sizeof("ALL SWITCHES")*6) / 2, 14, "ALL SWITCHES", screen);
		create_string(1, 64 - (sizeof("DOWN")*6) / 2, 22, "DOWN", screen);
		display_full_bin(screen);

		if (switches == 0x0) {
			menu();
		}
	}

	return 0;
}
