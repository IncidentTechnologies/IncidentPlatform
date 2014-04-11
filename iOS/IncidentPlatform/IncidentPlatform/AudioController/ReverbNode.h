//
//  Reverb.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//

#ifndef gTarAudioController_Reverb_h
#define gTarAudioController_Reverb_h

#include "EffectNode.h"
#include "DiffusionTankNode.h"
#include "FirstOrderFilterNode.h"

// Reverberator. Design taken from Jon Datorro's "Effective Design Part 1: 
// Reverberator and Other Filters". https://ccrma.stanford.edu/~dattorro/EffectDesignPart1.pdf
class ReverbNode : public EffectNode
{
public:
    ReverbNode(double wet);
    
    inline double InputSample(double sample);
    
    void Reset();
    void ClearOutEffect();
    
    bool SetBandwidth(double bandwidth);
    bool SetDamping(double damping);
    bool SetDecay(double decay);
    bool SetInputDiffusion1(double diffusion);
    bool SetInputDiffusion2(double diffusion);
    bool SetDecayDiffusion1(double diffusion);
    bool SetDecayDiffusion2(double diffusion);
    bool SetPreDelayLength(double scale);
    bool SetDelayL1Length(double length);
    bool SetDelayL2Length(double length);
    bool SetDelayR1Length(double length);
    bool SetDelayR2Length(double length);
    bool setPrimaryParam(float value);
    bool setSecondaryParam(float value);
    
    float GetNextSample(unsigned long int timestamp);
    
    Parameter* getPrimaryParam();
    Parameter* getSecondaryParam();
    Parameter* getLFO();
    Parameter* getExcursion();
    
    ~ReverbNode();
    
private:
    // Input Diffusers
    /*DiffusionTank* m_pInputDiffusionA;
    DiffusionTank* m_pInputDiffusionB;
    DiffusionTank* m_pInputDiffusionC;
    DiffusionTank* m_pInputDiffusionD;*/
    
    DiffusionTankNode* m_pInputDiffusionA;
    DiffusionTankNode* m_pInputDiffusionB;
    DiffusionTankNode* m_pInputDiffusionC;
    DiffusionTankNode* m_pInputDiffusionD;
    
    double m_pInputDiffusionA_delay;
    double m_pInputDiffusionB_delay;
    double m_pInputDiffusionC_delay;
    double m_pInputDiffusionD_delay;    
    double m_inputDiffusion1;
    double m_inputDiffusion2;
    
    // Reverberation Tank
    /*DiffusionTank* m_pDecayDiffusionL1;
    DiffusionTank* m_pDecayDiffusionL2;
    DiffusionTank* m_pDecayDiffusionR1;
    DiffusionTank* m_pDecayDiffusionR2;*/
    
    DiffusionTankNode* m_pDecayDiffusionL1;
    DiffusionTankNode* m_pDecayDiffusionL2;
    DiffusionTankNode* m_pDecayDiffusionR1;
    DiffusionTankNode* m_pDecayDiffusionR2;
    
    double m_pDecayDiffusionL1_delay;
    double m_pDecayDiffusionL2_delay;
    double m_pDecayDiffusionR1_delay;
    double m_pDecayDiffusionR2_delay;
    double m_decayDiffusion1;
    double m_decayDiffusion2;
    
    // Filters
    FirstOrderFilterNode* m_pBandWidthFilter;
    Parameter *m_pBandwidth;
    float m_currentBandwidth;
    FirstOrderFilterNode* m_pDampingFilterL;
    FirstOrderFilterNode* m_pDampingFilterR;
    double m_damping;
    
    // Delay lines
    double m_PreDelayLine_delay;
    double m_DelayLineL1_delay;
    double m_DelayLineL2_delay;
    double m_DelayLineR1_delay;
    double m_DelayLineR2_delay;
    long m_DelayLine_n;
    long m_DelayLine_c;
    double *m_pPreDelayLine;
    double *m_pDelayLineL1;
    double *m_pDelayLineL2;
    double *m_pDelayLineR1;
    double *m_pDelayLineR2;
    long m_PreDelayLine_l;
    long m_DelayLineL1_l;
    long m_DelayLineL2_l;
    long m_DelayLineR1_l;
    long m_DelayLineR2_l;
    
    Parameter *m_pLFOFreq;
    Parameter *m_pExcursion;
    Parameter *m_pDecay;
    float m_currentDecay;
    
    double max;
    double min;
};


#endif
