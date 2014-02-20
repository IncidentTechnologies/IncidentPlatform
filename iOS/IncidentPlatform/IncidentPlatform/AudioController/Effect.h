//
//  Effect.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//

#ifndef gTarAudioController_Effect_h
#define gTarAudioController_Effect_h

#include "Parameter.h"
#include <string.h>

class Effect {  
public:   
    Effect(std::string name, double wet, double SamplingFrequency);
    
    virtual double InputSample(double sample) = 0;
    
    bool SetPassThru(bool state);
    
    bool SetWet(double wet);
    
    float GetWet();
    
    virtual void Reset();
    
    virtual void ClearOutEffect();
    
    bool isOn();
    
    virtual ~Effect();
    
    std::string getName();
    virtual Parameter& getPrimaryParam();
    virtual bool setPrimaryParam(float value);
    virtual Parameter& getSecondaryParam();
    virtual bool setSecondaryParam(float value);
    
protected:
    std::string m_name;
    Parameter *m_pWet;
    bool m_fPassThrough;
    double m_SamplingFrequency;
};


#endif
