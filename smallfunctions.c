#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(){
  int switches;
  switches = (PORTD >> 8) & 0x000f;
  return switches;
}

int getbtns(){
  int buttons;
  buttons = (PORTD >> 5) & 0x0007;
  return buttons;
}

int getbtn1(){
  int button1;
  button1 = (PORTF >> 1) & 0x1;
}
