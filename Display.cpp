#include "Display.h"

Display::Display() {
    Graphics* g = new Graphics();
    graphics = g;

    LCD* l = new LCD();
    lcd = l;
}
