#include "AED.h"

AED::AED() {
    Display* d = new Display();
    display = d;

    isPoweredOn = false;
    batteryLevel = 100;
    totalTime = 0;

    //Slot to check if battery needs to be decremented after evry 10 seconds
    connect(display->getLCD()->getTimer(), SIGNAL(timeout()), this, SLOT(decrementBatteryLevel()));

}

void AED::handlePowerOn() {
    isPoweredOn = true;
    display->getLCD()->getTimer()->start(1000); //Start timer
    display->getLCD()->setMessage("UNIT OKAY");

    QTimer::singleShot(2000, this, [=]() {
        display->getGraphics()->illuminateGraphic(1);
        display->getLCD()->setMessage("CHECK RESPONSIVENESS");
    });

}

void AED::handlePowerOff() {
    isPoweredOn = false;
    // Stop the timer and remove messages on LCD
    display->getLCD()->getTimer()->stop();
    display->getLCD()->resetElapsedTime();
    display->getLCD()->setMessage("");

}

void AED::handleCheckResponsiveness() {
    display->getGraphics()->illuminateGraphic(2);
    display->getLCD()->setMessage("CALL FOR HELP");
}

void AED::handleCallForHelp() {
    display->getGraphics()->illuminateGraphic(3);
    display->getLCD()->setMessage("ATTACH DEFIB PADS TO PATIENT'S BARE CHEST");
}

void AED::handleAnalyze() {
    display->getGraphics()->illuminateGraphic(4);
    display->getLCD()->setMessage("DON'T TOUCH PATIENT, ANALYZING");
    QTimer::singleShot(5000, this, [=]() {
        //If CPR
        display->getGraphics()->illuminateGraphic(5);
        display->getLCD()->setMessage("START CPR");
        //Else If SHOCK
    });

}

void AED::decrementBatteryLevel() {
    if (isPoweredOn) {
        totalTime++;
        if (totalTime % 10 == 0) { // Decrement battery every 10 seconds
            batteryLevel = qMax(0, batteryLevel - 1); // Ensure the batteryLevel doesn't go below 0
            cout << to_string(batteryLevel) << endl;
            // Update UI
        }
    }
}

void AED::handleCompress() {

}

