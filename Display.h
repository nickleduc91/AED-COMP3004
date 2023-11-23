#ifndef DISPLAY_H
#define DISPLAY_H

#include "LCD.h"
#include "Graphics.h"
#include "ShockIndicator.h"

using namespace std;

class Display {
    public:
        Display();
        LCD* getLCD() { return lcd; }
        Graphics* getGraphics() { return graphics; }
        ShockIndicator* getIndicator() { return indicator; }

    private:
        LCD* lcd;
        Graphics* graphics;
        ShockIndicator* indicator;

};
#endif // DISPLAY_H
