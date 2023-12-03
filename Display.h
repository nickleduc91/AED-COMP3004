#ifndef DISPLAY_H
#define DISPLAY_H

#include "LCD.h"
#include "Graphics.h"

using namespace std;

class Display {
    public:
        Display();
        LCD* getLCD() { return lcd; }
        Graphics* getGraphics() { return graphics; }

    private:
        LCD* lcd;
        Graphics* graphics;

};
#endif // DISPLAY_H
