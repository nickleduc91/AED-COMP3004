#include "Graphics.h"
#include <QTimer>

Graphics::Graphics() {

}

void Graphics::illuminateGraphic(int step) {
    //Here we have the illumination functionality of the graphics
    emit callHandleIlluminateGraphic(step);
}
