#include "AED.h"

AED::AED() {
    Display* d = new Display();
    Electrode* e = new Electrode();
    display = d;
    electrode = e;

    isPoweredOn = false;
    batteryLevel = 100;
    totalTime = 0;
    voltage = 10;
    shockCount = 0;

    //Populate ECG data for victim
    ecgIndex = 0;
    std::srand(std::time(0));
    for(int i = 0; i < 100; i++) {
        //Generate bit randomly to decide what ecg value we are on
        int randomNumber = std::rand() % 4;
        victimECG.push_back(randomNumber);
    }


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

void AED::handleNewBatteries() {
    display->getLCD()->getTimer()->stop();
    display->getLCD()->resetElapsedTime();
    resetTotalTime();
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
    display->getLCD()->setMessage("DON'T TOUCH PATIENT, ANALYZING");
    QTimer::singleShot(1500, this, [=]() {
        if(isArythmia()) {
            //Enable the shock button
            display->getLCD()->setMessage("SHOCK ADVISED");
            display->getGraphics()->illuminateGraphic(6);
        } else {
            //Perform CPR if shock is not issued
            display->getLCD()->setMessage("NO SHOCK ADVISED");
            display->getGraphics()->illuminateGraphic(5);
            setDelayedMessage("START CPR", 1500);
        }

    });

}

void AED::handleShock() {
    display->getLCD()->setMessage("SHOCK WILL BE DELIVERED IN 3, 2, 1");
    electrode->shock(voltage);
    shockCount++;
    display->getLCD()->updateShockCount(shockCount);
    setDelayedMessage("SHOCK DELIVERED", 1500);
    //Start CPR after shock is issued
    display->getGraphics()->illuminateGraphic(5);
    setDelayedMessage("START CPR", 1500);

}

void AED::setDelayedMessage(const string message, int delay) {
    QTimer::singleShot(delay, this, [=]() {
        display->getLCD()->setMessage(message);
    });
}

void AED::handleAttach(bool left, bool right, bool back, bool ripped, bool towel, bool clip) {

    if(checkPads(left, right, back, ripped, towel, clip)) {
        display->getGraphics()->illuminateGraphic(4);
    }
}

void AED::decrementBatteryLevel() {
    if (isPoweredOn) {
        totalTime++;
        if (totalTime % 10 == 0) { // Decrement battery every 10 seconds
            batteryLevel = qMax(0, batteryLevel - 1); // Ensure the batteryLevel doesn't go below 0
            if (batteryLevel == 0){
                emit deadBattery();
            } else {
                emit updateBatteryLevel(batteryLevel);
            }
        }
    }
}

void AED::handleCompress(bool pressedHardEnough) {
    display->getLCD()->setMessage("START CPR");
    if (pressedHardEnough) {
        setDelayedMessage("CONTINUE CPR", 1250);
        QTimer::singleShot(5000, this, [=]() {
            display->getLCD()->setMessage("STOP CPR");
            QTimer::singleShot(2000, this, [=]() {
                display->getLCD()->setMessage("GIVE TWO BREATHS");
                display->getGraphics()->illuminateGraphic(7);
            });
        });
    } else {
        setDelayedMessage("PUSH HARDER", 1250);
        QTimer::singleShot(3500, this, [=]() {
            emit pushHarder();
        });
    }
}

void AED::handleBreathe() {
    ecgIndex++; //Increment the index so we can get the next rhythm before analyzing
    QTimer::singleShot(4000, this, [=]() {
        display->getGraphics()->illuminateGraphic(4);
    });
}

void AED::failedSelfTest() {
    display->getLCD()->setMessage("UNIT FAILED");
}

bool AED::checkPads(bool left, bool right, bool back, bool ripped, bool towel, bool clip){

    //Only use electrodes labeled “Infant/Child” on children less than 8 years old or weighing less than
    //55 lbs (25 kg). Use CPR-D-padz® if victim is older than 8 years or weighs more than 55 lbs (25 kg)

    //if adult - bool left, bool right
    //if child - bool right or bool left, and bool back
    //if overweight - book ripped, bool left, bool right

    if(!isVictimAdult){
        if(((back && left && !right) || (back && right && !left)) && ( (isVictimWet && towel) || !isVictimWet) && ( (isVictimHairy && clip) || !isVictimHairy) ){
            display->getLCD()->setMessage("PEDIATRIC PADS");
            return true;
        }else{
            display->getLCD()->setMessage("CHECK ELECTRODE PADS");
            return false;
        }
    }else{
        if(isVictimOverWeight){
            if(ripped && right && left && ( (isVictimWet && towel) || !isVictimWet) && ( (isVictimHairy && clip) || !isVictimHairy)){
                display->getLCD()->setMessage("FAT ASS ATTACHED SUCCESSFULLY");
                return true;
            }else{
                display->getLCD()->setMessage("CHECK ELECTRODE PADS");
                return false;
            }
        }else{
            if(left && right && ( (isVictimWet && towel) || !isVictimWet) && ( (isVictimHairy && clip) || !isVictimHairy)){
                display->getLCD()->setMessage("ADULT PADS");
                return true;
            }else{
                display->getLCD()->setMessage("CHECK ELECTRODE PADS");
                return false;
            }
        }
    }
    return false;

}

bool AED::performSelfTest(bool defibConnection,bool ecgCircuitry,bool defibCharge,bool microprocessor,bool cprCircuitrySensor,bool audioCircuitry) {
    string statusMessage = "FAILED";
    if(batteryLevel > 10 && !defibConnection && !ecgCircuitry && !defibCharge && !microprocessor && !cprCircuitrySensor && !audioCircuitry){
        isPassedTest = true;
        statusMessage = "PASSED";
    }else {
        isPassedTest = false;
    }
    emit callHandleStatusUpdate(statusMessage,isPassedTest);
    return isPassedTest;
}

void AED::setVictim(int age, int weight, bool isHairy, bool isWet) {
    victimAge = age;
    victimWeight = weight;
    isVictimHairy = isHairy;
    isVictimWet = isWet;
    if(age < 8){
        isVictimAdult = false;
        voltage = 5; //Adjust voltage to child
    }else{
        isVictimAdult = true;
        voltage = 10; //Adjust voltage to adult
        if(weight > 250){
            isVictimOverWeight = true;
        }else{
            isVictimOverWeight = false;
        }
    }
}
