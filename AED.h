#ifndef AED_H
#define AED_H

#include <string>
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
        AED();
        bool performSelfTest(bool ecgCircuitry,bool defibCharge,bool microprocessor,bool cprCircuitrySensor,bool audioCircuitry);
        void handlePowerOn();
        void handleCheckResponsiveness();
        void handleCallForHelp();
        void handleAnalyze();
        void handleAttach(bool left, bool right, bool back, bool ripped, bool towel, bool clip);
        void handleCompress(bool);
        void handleBreathe();
        void handlePowerOff();
        void handleShock();
        void handleNewBatteries();
        void failedSelfTest();
        void handlePlugInOutElectrode();
        void handleRhythmChange(int rhythm) { victimECG.at(ecgIndex) = rhythm; }

        bool checkPads(bool left, bool right, bool back, bool ripped, bool towel, bool clip); // Check if the pads were attached properly

        void setVictim(int age, int weight, bool isHairy, bool isWet);
        void setDelayedMessage(const string message, int delay);

        void setIsPoweredOn(bool val) { isPoweredOn = val; }
        bool isOn() { return isPoweredOn; }
        int getRhythm() { return (victimECG.at(ecgIndex)); }

        void setBatteryLevel(int val) { batteryLevel = val; }
        void resetTotalTime() { totalTime = 0; }

        bool isAdult() { return isVictimAdult; }

        Display* display;
        Electrode* electrode;

    private:

        bool isPassedTest;
        bool isPoweredOn;

        //Self-test variables
        int batteryLevel;

        //Victim values
        int victimAge; //0-8 = child, 9 >= adult
        int victimWeight;
        bool isVictimWet;
        bool isVictimHairy;
        bool isVictimOverWeight; //Break pads or not
        bool isVictimAdult;
        vector<int> victimECG; //Contains a bit for every type of rhythm: Normal - 0, VFib - 1, VTach - 2, Flat line - 3
        int ecgIndex;
        int voltage;

        //Display values
        int graphicStage;
        int shockCount;
        int totalTime;

        int currentStep;

    private slots:
        void decrementBatteryLevel();

    signals:
        void callHandleStatusUpdate(string message,bool status);
        void updateBatteryLevel(int);
        void deadBattery();
        void pushHarder();
        void vfib_graph_signal();
        void vtac_graph_signal();
        void normal_graph_signal();
        void flatline_graph_signal();
        void enableRhythms(bool enable);
};
#endif // AED_H
