#include "LCD.h"

LCD::LCD()
    : timer(new QTimer(this)),
      elapsedTime(QTime(0, 0))
{
    // Here we have the constructor of the LCD
    // It includes the timer  funcctionality
    // Connect the timer timeout signal to the updateElapsedTime slot
    connect(timer, SIGNAL(timeout()), this, SLOT(updateElapsedTime()));
}

void LCD::setMessage(string message) {
    // Here we handle sending messages to the LCD
    emit callHandlelogToDisplay(message, "");
}

void LCD::resetElapsedTime() {
    // Here we handle reset the time elapsed of the AED
    elapsedTime = QTime(0, 0);
    emit callHandlelogToDisplay(elapsedTime.toString("mm:ss").toStdString(), "time"); // Format as "XX:XX"
}

void LCD::updateElapsedTime() {
    // Here We Update the elapsed time
    elapsedTime = elapsedTime.addSecs(1); // Increment elapsed time by 1 second

    // Emit the signal with the formatted elapsed time
    emit callHandlelogToDisplay(elapsedTime.toString("mm:ss").toStdString(), "time"); // Format as "XX:XX"
}

void LCD::updateShockCount(int shockCount) {
    // Here we handle updating the shock count of the AED to the LCD
    emit callHandlelogToDisplay(to_string(shockCount), "shock");
}
