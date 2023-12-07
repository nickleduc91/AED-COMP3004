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
    while (true) {
        // Generate a random number with a bias towards VTach, VFib, and Flatline
        int randomNumber;
        if (std::rand() % 10 < 3) {  // 30% chance for 0 ("Normal")
            randomNumber = 0;
        } else {
            randomNumber = 1 + std::rand() % 3;  // 70% chance for 1, 2, or 3
        }
        victimECG.push_back(randomNumber);
        if(randomNumber == 0) break;
    }

    //Slot to check if battery needs to be decremented after evry 10 seconds
    connect(display->getLCD()->getTimer(), SIGNAL(timeout()), this, SLOT(decrementBatteryLevel()));

}

void AED::handlePlugInOutElectrode() {
    if(electrode->isElectrodePluggedIn()) {
        cout << "ELECTRODE: Unplugged" << endl;
        electrode->setElectrodePluggedIn(false);
        if(isPoweredOn) {
            display->getLCD()->setMessage("PLUG IN CABLE");
            if(currentStep == 4) {
                display->getGraphics()->disableStep(4);
            } else if(currentStep == 6) {
                display->getGraphics()->disableStep(6);
            } else if(currentStep == 5) {
                display->getGraphics()->disableStep(5);
            }
        }
    } else {
        electrode->setElectrodePluggedIn(true);
        cout << "ELECTRODE: Plugged In" << endl;
        if(isPoweredOn) {
            if(currentStep == 1) {
                display->getLCD()->setMessage("CHECK RESPONSIVENESS");
            } else if(currentStep == 2) {
                display->getLCD()->setMessage("CALL FOR HELP");
            } else if(currentStep == 3) {
                display->getLCD()->setMessage("ATTACH DEFIB PADS TO PATIENT'S BARE CHEST");
            } else if(currentStep == 4) {
                isAdult() ? display->getLCD()->setMessage("ADULT PADS") : display->getLCD()->setMessage("PEDIATRIC PADS");
                display->getGraphics()->illuminateGraphic(4);
            } else if(currentStep == 6){
                handleAnalyze();
            } else if(currentStep == 5) {
                display->getLCD()->setMessage("START CPR");
                display->getGraphics()->illuminateGraphic(5);
            } else if(currentStep == 7) {
                display->getLCD()->setMessage("GIVE TWO BREATHS");
            }
        }
    }
}

void AED::handlePowerOn() {
    isPoweredOn = true;
    cout << "AED: Powered On" << endl;
    display->getLCD()->getTimer()->start(1000); //Start timer
    display->getLCD()->setMessage("UNIT OKAY");
    currentStep = 1;

    QTimer::singleShot(2000, this, [=]() {
        display->getGraphics()->illuminateGraphic(1);
        display->getLCD()->setMessage("CHECK RESPONSIVENESS");
    });

}

void AED::handlePowerOff() {
    cout << "AED: Powered Off" << endl;
    isPoweredOn = false;
    currentStep = 0;
    // Stop the timer and remove messages on LCD
    display->getLCD()->getTimer()->stop();
    display->getLCD()->resetElapsedTime();
    display->getLCD()->setMessage("");
    display->getLCD()->resetECG();

}

void AED::handleNewBatteries() {
    cout << "AED: Batteries changed" << endl;
    display->getLCD()->getTimer()->stop();
    display->getLCD()->resetElapsedTime();
    resetTotalTime();
    display->getLCD()->setMessage("");
}

void AED::handleCheckResponsiveness() {
    cout << "RESCUER: Checks responsiveness" << endl;
    display->getGraphics()->illuminateGraphic(2);
    currentStep = 2;
    display->getLCD()->setMessage("CALL FOR HELP");
}

void AED::handleCallForHelp() {
    cout << "RESCUER: Calls for help" << endl;
    display->getGraphics()->illuminateGraphic(3);
    currentStep = 3;
    display->getLCD()->setMessage("ATTACH DEFIB PADS TO PATIENT'S BARE CHEST");
}

void AED::handleAnalyze() {
    cout << "AED: Analyzing" << endl;
    currentStep = 4;
    display->getLCD()->setMessage("DON'T TOUCH PATIENT, ANALYZING");
    QTimer::singleShot(1500, this, [=]() {
        if(getRhythm() == 1 || getRhythm() == 2) {

            if(getRhythm() == 1) {
                cout << "AED: Ventricular fibrillation rhythm detected" << endl;
                emit vfib_graph_signal();
            } else if(getRhythm() == 2) {
                cout << "AED: Ventricular tachycardia rhythm detected" << endl;
                emit vtac_graph_signal();
            }

            //Enable the shock button
            currentStep = 6;
            display->getLCD()->setMessage("SHOCK ADVISED");
            display->getGraphics()->illuminateGraphic(6);
        } else {

            if(getRhythm() == 0) {
                cout << "AED: Sinus rhythm detected" << endl;
                emit normal_graph_signal();
            } else if(getRhythm() == 3) {
                cout << "AED: Flat line rhythm detected" << endl;
                emit flatline_graph_signal();
            }

            //Perform CPR if shock is not issued
            currentStep = 5;
            display->getLCD()->setMessage("NO SHOCK ADVISED");
            display->getGraphics()->illuminateGraphic(5);
            setDelayedMessage("START CPR", 1500);
        }

    });

}

void AED::handleShock() {
    cout << "RESCUER: Delivers shock" << endl;
    currentStep = 5;
    display->getLCD()->setMessage("SHOCK WILL BE DELIVERED IN 3, 2, 1");
    electrode->shock(voltage);
    shockCount++;
    display->getLCD()->updateShockCount(shockCount);
    setDelayedMessage("SHOCK DELIVERED", 1500);
    //Start CPR after shock is issued
    QTimer::singleShot(1500, this, [=]() {
        display->getGraphics()->illuminateGraphic(5);
        display->getLCD()->setMessage("START CPR");
    });

}

void AED::setDelayedMessage(const string message, int delay) {
    QTimer::singleShot(delay, this, [=]() {
        display->getLCD()->setMessage(message);
    });
}

void AED::handleAttach(bool left, bool right, bool back, bool ripped, bool towel, bool clip) {

    if(checkPads(left, right, back, ripped, towel, clip)) {
        if(isAdult()) {
            if(isVictimOverWeight) {
                cout << "RESCUER: Attached electrode pads to overweight victim" << endl;
            } else {
                cout << "RESCUER: Attached electrode pads to adult victim" << endl;
            }
        } else if(!isAdult()) {
            cout << "RESCUER: Attached electrode pads to child victim" << endl;
        }

        currentStep = 4;
        if(electrode->isElectrodePluggedIn()) {
            display->getGraphics()->illuminateGraphic(4);
        } else {
            display->getGraphics()->disableStep(4);
        }
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
    cout << "RESCUER: Performs compressions" << endl;
    currentStep = 5;
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
    currentStep = 7;
}

void AED::handleBreathe() {
    cout << "RESCUER: Delivers 2 breaths" << endl;
    currentStep = 7;
    //Victim has a sinus heart rhythm and no longer needs the AED
    if(getRhythm() == 0) {
        handlePowerOff();
    } else {
        ecgIndex++; //Increment the index so we can get the next rhythm before analyzing
        QTimer::singleShot(4000, this, [=]() {
            if(electrode->isElectrodePluggedIn()) {
                display->getGraphics()->illuminateGraphic(4);
            } else {
                display->getGraphics()->disableStep(4);
            }

            currentStep = 4;
        });
    }

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
                display->getLCD()->setMessage("ADULT PADS");
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

bool AED::performSelfTest(bool ecgCircuitry,bool defibCharge,bool microprocessor,bool cprCircuitrySensor,bool audioCircuitry) {
    string statusMessage = "FAILED";
    if(batteryLevel > 10 && electrode->isElectrodePluggedIn() && !ecgCircuitry && !defibCharge && !microprocessor && !cprCircuitrySensor && !audioCircuitry){
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
