#ifndef ELECTRODE_H
#define ELECTRODE_H
using namespace std;

#include <string>
#include <iostream>

class Electrode {
    public:
        Electrode();
        void shock(int voltage);

        void plugInElectrode();
        void unplugElectrode();

        bool isElectrodePluggedIn() { return isElectrodeAttachedProperly; }
        void setElectrodePluggedIn(bool status) { isElectrodeAttachedProperly = status; }

    private:
       bool isElectrodeAttachedProperly;


};
#endif // ELECTRODE_H
