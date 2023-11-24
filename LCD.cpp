#include "LCD.h"

LCD::LCD() {

}

void LCD::setMessage(string message) {
    emit callHandlelogToDisplay(message);
}
