//
//  Parameter.h
//  gTarAudioController
//
//  This class represents a single parameter on an effect or module
//
//  Created by Franco Cedano on 11/23/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//

#ifndef gTarAudioController_Parameter_h
#define gTarAudioController_Parameter_h

#include <string>

class Parameter {
public:
    Parameter(float initial, float min, float max, const std::string& desc);
    
    float getValue();
    bool setValue(float newVal);
    
    float getMax();
    float getMin();
    std::string getName();
    
private:
    float m_value;
    float m_max;
    float m_min;
    std::string m_name;
};

#endif
