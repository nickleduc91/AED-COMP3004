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

    private:
       bool isElectrodeAttachedProperly;


};
#endif // ELECTRODE_H
