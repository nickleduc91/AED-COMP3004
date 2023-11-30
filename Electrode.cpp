#include <Electrode.h>

Electrode::Electrode(){
    isElectrodeAttachedProperly = true;
}

void Electrode::shock(int voltage){
    cout << "SHOCK of " << voltage << " volts administered." << endl;
}

void Electrode::plugInElectrode(){

}

void Electrode::unplugElectrode(){

}

