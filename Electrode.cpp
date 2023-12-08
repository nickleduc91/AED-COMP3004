#include <Electrode.h>

Electrode::Electrode(){
    //Here is the constructor of the electrodes
    isElectrodeAttachedProperly = true;
}

void Electrode::shock(int voltage){
    //Here we administer the shock with the correct amount of voltage

    cout << "ELECTRODE: Shock of " << voltage << " volts administered" << endl;
}
