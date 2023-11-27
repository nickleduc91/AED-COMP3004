#include "AED.h"

AED::AED() {
    Display* d = new Display();
    Electrode* e = new Electrode();
    display = d;
    electrode = e;

    isPoweredOn = false;
    batteryLevel = 100;
    totalTime = 0;

    //Populate ECG data for victim
    std::srand(std::time(0));
    // Generate a random number in the range from 0 to 3
    for(int i = 0; i < 100; i++) {
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
//    QTimer::singleShot(5000, this, [=]() {
//        //If CPR
//            //display->getGraphics()->illuminateGraphic(5);
//            //display->getLCD()->setMessage("START CPR");
//        //Else If SHOCK
//        if(checkPads(left, right, back, ripped, age, weight)){
//            //electrode->shock(10);
//        }

//    });

}

void AED::handleAttach(bool left, bool right, bool back, bool ripped) {

    if(checkPads(left, right, back, ripped)) {
        display->getGraphics()->illuminateGraphic(4);
    }
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

bool AED::checkPads(bool left, bool right, bool back, bool ripped){

    //Only use electrodes labeled “Infant/Child” on children less than 8 years old or weighing less than
    //55 lbs (25 kg). Use CPR-D-padz® if victim is older than 8 years or weighs more than 55 lbs (25 kg)

    //if adult - bool left, bool right
    //if child - bool right or bool left, and bool back
    //if overweight - book ripped, bool left, bool right

    if(!isVictimAdult){
        if((back && left && !right) || (back && right && !left)){
            display->getLCD()->setMessage("PEDIATRIC PADS");
            return true;
        }else{
            display->getLCD()->setMessage("CHECK ELECTRODE PADS");
            return false;
        }
    }else{
        if(isVictimOverWeight){
            if(ripped && right && left){
                display->getLCD()->setMessage("FAT ASS ATTACHED SUCCESSFULLY");
                return true;
            }else{
                display->getLCD()->setMessage("CHECK ELECTRODE PADS");
                return false;
            }
        }else{
            if(left && right){
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
    if(batteryLevel > 20 && !defibConnection && !ecgCircuitry && !defibCharge && !microprocessor && !cprCircuitrySensor && !audioCircuitry){
        isPassedTest = true;
        statusMessage = "PASSED";
    }else {
        isPassedTest = false;
    }
    emit callHandleStatusUpdate(statusMessage,isPassedTest);
    return isPassedTest;
}

void AED::setVictim(int age, int weight) {
    victimAge = age;
    victimWeight = weight;
    if(age < 8 || weight < 55){
        isVictimAdult = false;
    }else{
        isVictimAdult = true;
        if(weight > 250){
            isVictimOverWeight = true;
        }else{
            isVictimOverWeight = false;
        }
    }
}
