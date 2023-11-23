#ifndef AED_H
#define AED_H

#include <string>
#include "ControlPanel.h"
#include "Display.h"
#include "Electrode.h"

using namespace std;

class AED {
    public:
        AED(bool, bool, bool , bool, bool, bool, bool, int age, int weight, bool dry, bool hairy);
        bool performSelfTest();

        void emitShock();
        void analyzeHeart();
        void checkPads(bool left, bool right, bool back, bool ripped); // Check if the pads were attached properly


    private:
        ControlPanel* cp;
        Display* display;
        Electrode* electrode;

        bool isPassedTest;

        //Self-test variables
        bool batteryFull;
        bool electrodeConnected;
        bool ecgCircuitry;
        bool defibCharge;
        bool microprocessor;
        bool cprCircuitry;
        bool audioCircuitry;

        //Victim values
        int victimAge; //0-8 = child, 9 >= adult
        int victimWeight;
        bool isChestDry;
        bool isChestHairy;
        bool isVictimOverWeight; //Break pads or not

        //Display values
        int graphicStage;
        int shockCount;
        int elapsedTime;

        int currentStep;

        // display->getGraphics()->illuminateGraphic(currentStep);
        // curentStep++;



};
#endif // AED_H
