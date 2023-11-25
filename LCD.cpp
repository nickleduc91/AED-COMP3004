#include "LCD.h"

LCD::LCD()
    : timer(new QTimer(this)),
      elapsedTime(QTime(0, 0))
{
    // Connect the timer timeout signal to the updateElapsedTime slot
    connect(timer, SIGNAL(timeout()), this, SLOT(updateElapsedTime()));
}

void LCD::setMessage(string message) {
    emit callHandlelogToDisplay(message, "");
}

void LCD::resetElapsedTime() {
    elapsedTime = QTime(0, 0);
    emit callHandlelogToDisplay(elapsedTime.toString("mm:ss").toStdString(), "time"); // Format as "XX:XX"
}

void LCD::updateElapsedTime() {
    // Update the elapsed time
    elapsedTime = elapsedTime.addSecs(1); // Increment elapsed time by 1 second

    // Emit the signal with the formatted elapsed time
    emit callHandlelogToDisplay(elapsedTime.toString("mm:ss").toStdString(), "time"); // Format as "XX:XX"
}
