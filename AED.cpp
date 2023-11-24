#include "AED.h"

AED::AED() {
    Display* d = new Display();
    display = d;

}

void AED::handlePowerOn() {
    display->getLCD()->setMessage("UNIT OKAY");

    QTimer::singleShot(2000, this, [=]() {
        display->getGraphics()->illuminateGraphic(1);
        display->getLCD()->setMessage("CHECK RESPONSIVENESS");
    });

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
}
