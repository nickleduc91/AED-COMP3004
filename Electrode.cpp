#include <Electrode.h>

Electrode::Electrode(){
    isElectrodeAttachedProperly = false;
}

void Electrode::shock(int voltage){
    cout << "SHOCK of " << voltage << " volts administered." << endl;
}

void Electrode::plugInElectrode(){

}

void Electrode::unplugElectrode(){

}

