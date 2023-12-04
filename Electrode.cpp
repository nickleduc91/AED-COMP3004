#include <Electrode.h>

Electrode::Electrode(){
    isElectrodeAttachedProperly = true;
}

void Electrode::shock(int voltage){
    cout << "ELECTRODE: Shock of " << voltage << " volts administered" << endl;
}
