#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H
using namespace std;

class ControlPanel {
    public:
        ControlPanel();
        void powerOn();
        void powerOff();

        void updateStatus();
        void changeBatteries();

    private:
        bool status;

};
#endif // CONTROLPANEL_H
