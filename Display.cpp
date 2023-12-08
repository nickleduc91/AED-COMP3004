#include "Display.h"

Display::Display() {
    //Here is the constructor for the display of the AED
    //We have the graphics and the lcd
    Graphics* g = new Graphics();
    graphics = g;

    LCD* l = new LCD();
    lcd = l;
}
