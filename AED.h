#ifndef AED_H
#define AED_H

#include <string>
#include "ControlPanel.h"
#include "Display.h"
#include "Electrode.h"
#include <vector>
#include <QTimer>
#include <QObject>
#include <cstdlib>

using namespace std;

class AED : public QObject {
    Q_OBJECT
    public:
        AED(bool, bool, bool , bool, bool, bool, bool, int age, int weight, bool dry, bool hairy);
        AED();
        bool performSelfTest(bool defibConnection,bool ecgCircuitry,bool defibCharge,bool microprocessor,bool cprCircuitrySensor,bool audioCircuitry);
        void handlePowerOn();
        void handleCheckResponsiveness();
        void handleCallForHelp();
        void handleAnalyze();
        void handleAttach(bool left, bool right, bool back, bool ripped);
        void handleCompress();
        void handlePowerOff();

        void emitShock();
        void analyzeHeart();
        bool checkPads(bool left, bool right, bool back, bool ripped); // Check if the pads were attached properly
        void checkResponsiveness();

        void setVictim(int age, int weight);

        bool isOn() { return isPoweredOn; }

        ControlPanel* cp;
        Display* display;
        Electrode* electrode;

    private:

        bool isPassedTest;
        bool isPoweredOn;

        //Self-test variables
        int batteryLevel;
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
        bool isVictimAdult;
        vector<int> victimECG; //Contains a bit for every type of rhythm: Normal - 0, VFib - 1, VTach - 2, Flat line - 3

        //Display values
        int graphicStage;
        int shockCount;
        int totalTime;

        int currentStep;

    private slots:
        void decrementBatteryLevel();

    signals:
        void callHandleStatusUpdate(string message,bool status);


};
#endif // AED_H
