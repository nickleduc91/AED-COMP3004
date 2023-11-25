#include "AED.h"

AED::AED() {
    Display* d = new Display();
    Electrode* e = new Electrode();
    display = d;
    electrode = e;

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

void AED::handleAnalyze(bool left, bool right, bool back, bool ripped, int age, int weight) {

    display->getGraphics()->illuminateGraphic(4);
    display->getLCD()->setMessage("DON'T TOUCH PATIENT, ANALYZING");
    QString victimStats = "VICTIM AGE = " + QString::number(age) + " VICTIM WEIGHT = " + QString::number(weight);
    display->getLCD()->setMessage(victimStats.QString::toStdString());
    QTimer::singleShot(5000, this, [=]() {
        //If CPRS
            //display->getGraphics()->illuminateGraphic(5);
            //display->getLCD()->setMessage("START CPR");
        //Else If SHOCK
        if(checkPads(left, right, back, ripped, age, weight)){
            //electrode->shock(10);
        }

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

bool AED::checkPads(bool left, bool right, bool back, bool ripped, int age, int weight){

    //Only use electrodes labeled “Infant/Child” on children less than 8 years old or weighing less than
    //55 lbs (25 kg). Use CPR-D-padz® if victim is older than 8 years or weighs more than 55 lbs (25 kg)

    //if adult - bool left, bool right
    //if child - bool right or bool left, and bool back
    //if overweight - book ripped, bool left, bool right

    if(age < 8 || weight < 55){
        if((back && left && !right) || (back && right && !left)){
            display->getLCD()->setMessage("CHILD ATTACHED SUCCESSFULLY");
            return true;
        }else{
            display->getLCD()->setMessage("CHILD ATTACHED FAILED");
            return false;
        }
    }else{
        if(weight > 250){
            if(ripped && right && left){
                display->getLCD()->setMessage("FAT ASS ATTACHED SUCCESSFULLY");
                return true;
            }else{
                display->getLCD()->setMessage("FAT ASS ATTACHED FAILED");
                return false;
            }
        }else{
            if(left && right){
                display->getLCD()->setMessage("ADULT ATTACHED SUCCESSFULLY");
                return true;
            }else{
                display->getLCD()->setMessage("ADULT ATTACHED FAILED");
                return false;
            }
        }
    }
    return false;

}
