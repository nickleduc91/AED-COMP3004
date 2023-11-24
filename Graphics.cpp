#include "Graphics.h"
#include <QTimer>

Graphics::Graphics() {

}

void Graphics::illuminateGraphic(int step) {
    emit callHandleIlluminateGraphic(step);
}
