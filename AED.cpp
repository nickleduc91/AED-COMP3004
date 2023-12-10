#include "AED.h"

AED::AED() {
    //AED Constructor
    Display* d = new Display();
    Electrode* e = new Electrode();
    display = d;
    electrode = e;
    //Setting Default Values
    isPoweredOn = false;
    batteryLevel = 100;
    totalTime = 0;
    voltage = 10;
    shockCount = 0;

    //Populate ECG data for victim (heart rhythhm sequences)
    ecgIndex = 0;
    std::srand(std::time(0));
    for (int i = 0; i < 100; i++) {
        // Generate a random number with a bias towards VTach, VFib, and Flatline
        int randomNumber;
        if (std::rand() % 10 < 3) {  // 30% chance for 0 ("Normal")
            randomNumber = 0;
        } else {
            randomNumber = 1 + std::rand() % 3;  // 70% chance for 1, 2, or 3
        }
        victimECG.push_back(randomNumber);
    }

    //Slot to check if battery needs to be decremented after evry 10 seconds
    connect(display->getLCD()->getTimer(), SIGNAL(timeout()), this, SLOT(decrementBatteryLevel()));

}

void AED::handlePlugInOutElectrode() {
    //Handles what happens when the electrodes of the AED are unplugged
    //With not electrode plugged in, we cannot use the AED, so we disable steps that require analyzing
    //If electrodes  are  plugged in, every step will work
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
    //Turns on AED
    //Begins Timer
    //Illumniates first step, check responsiveness
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
    //Turns off AED
    //Resets current step
    //Resets ECG
    //Clears display
    //Stops time
    cout << "AED: Powered Off" << endl;
    isPoweredOn = false;
    currentStep = 0;
    display->getLCD()->updateShockCount(0);
    shockCount = 0;
    // Stop the timer and remove messages on LCD
    display->getLCD()->getTimer()->stop();
    display->getLCD()->resetElapsedTime();
    display->getLCD()->setMessage("");
    display->getLCD()->resetECG();

}

void AED::handleNewBatteries() {
    //When batteries are changed
    //Reset AED's time

    cout << "AED: Batteries changed" << endl;
    display->getLCD()->getTimer()->stop();
    display->getLCD()->resetElapsedTime();
    resetTotalTime();
    display->getLCD()->setMessage("");
}

void AED::handleCheckResponsiveness() {
    //This is when the user (Rescuer) checks responsiveness of the patient
    //We highlight the next step, the correct graphic and prompt the LCD

    cout << "RESCUER: Checks responsiveness" << endl;
    display->getGraphics()->illuminateGraphic(2);
    currentStep = 2;
    display->getLCD()->setMessage("CALL FOR HELP");
}

void AED::handleCallForHelp() {
    //This is when the user (Rescuer) calls for help for the patient
    //We highlight the next step, the correct graphic and prompt the LCD
    cout << "RESCUER: Calls for help" << endl;
    display->getGraphics()->illuminateGraphic(3);
    currentStep = 3;
    display->getLCD()->setMessage("ATTACH DEFIB PADS TO PATIENT'S BARE CHEST");
}

void AED::handleAnalyze() {
    //This is when the user (Rescuer) begins analyzing the patient
    //We get the type of heart rhymthm (originally sequenced in the constructor)
    //Depdending on type of heart rhythm detected, we output the corresponding ECG graph

    //We prompt the user (Rescuer) an advisory to shock the patient if experiencing fibrillation or tachycardia
    //The AED then illminates the correct graphic and leads the user to the next step

    //We prompt the user (Rescuer) an advisory to perform CPR (compression and breaths) too the patient if experiencing sinus or flatline rhythms
    //The AED then illminates the correct graphic and leads the user to the next step

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
    emit enableRhythms(false);

}

void AED::handleShock() {
    //Here we handle what happens when the user delivers a shock
    //We manage each shock in the shock count
    //We direct them to the next step, CPR, after shock is issued

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
    //This is a helper function to display text with a delay
    QTimer::singleShot(delay, this, [=]() {
        display->getLCD()->setMessage(message);
    });
}

void AED::handleAttach(bool left, bool right, bool back, bool ripped, bool towel, bool clip) {
    //This is a function to handle attaching pads
    //We ensure that the pads are attached correctly, depending on age, weight, wetness, and hair
    //Once we attach, we move to the next step

    if(checkPads(left, right, back, ripped, towel, clip)) {
        emit enableRhythms(true);
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
        if (totalTime % 2 == 0) { // Decrement battery every 10 seconds
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
        ecgIndex++; //Increment the index so we can get the next rhythm before analyzing
        handlePowerOff();
    } else {
        ecgIndex++; //Increment the index so we can get the next rhythm before analyzing
        QTimer::singleShot(4000, this, [=]() {
            if(electrode->isElectrodePluggedIn()) {
                display->getGraphics()->illuminateGraphic(4);
            } else {
                display->getGraphics()->disableStep(4);
            }
            emit enableRhythms(true);

            currentStep = 4;
        });
    }

}

void AED::failedSelfTest() {
    display->getLCD()->setMessage("UNIT FAILED");
}

bool AED::checkPads(bool left, bool right, bool back, bool ripped, bool towel, bool clip){

    //This is a helper function to see if the pads are attached correctly
    //We take into account the patient's weight, age, their wetness, and hair

    //The attributes checked are as follows

    //If victim is an adult, we need attachment to the right and left pads to the chest
    //If the victim is a child, we need attachment either to the right and back or left and back

    //If the victim is overweight, we need to rip the electrodes and attach them

    //If the victim is wet, we need to dry them with a towel
    //If the victim is hairy, we need to clip their hair off

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
    //This performs a self test to see if the AED is fully functional, performed at the start of each power on
    //It requires that the electrodes are plugged in, functional ECG circuitry, functional microprocesor, functional audio circuitry, a functional circuitry sensor, and charge in the defibrillator

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
    //Here is where we set the victim of the AED
    //We correct the voltage depending on age

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
        if(weight >= 250){
            isVictimOverWeight = true;
        }else{
            isVictimOverWeight = false;
        }
    }
}
