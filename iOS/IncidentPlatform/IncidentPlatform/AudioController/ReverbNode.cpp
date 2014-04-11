#include "ReverbNode.h"
#include <math.h>

ReverbNode::ReverbNode(double wet):
    //Effect("Reverb", wet, SamplingFrequency),
    EffectNode(),
    max(0),
    min(0)
{
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
    
    SetWet(wet);
    
    m_pLFOFreq = new Parameter(1.2, 0.0, 10, "Delay Line LFO");
    m_pExcursion = new Parameter(27, 0.0, 50, "Excursion");
    m_pDecay = new Parameter(0.15, 0.0, 0.97, "Decay");
    m_currentDecay = m_pDecay->getValue();
    m_pBandwidth = new Parameter(0.75, 0.0, 1.0, "Bandwidth");
    m_currentBandwidth = m_pBandwidth->getValue();
    
    m_pInputDiffusionA_delay = 4.771;
    m_pInputDiffusionB_delay = 3.595;
    m_pInputDiffusionC_delay = 12.735;
    m_pInputDiffusionD_delay = 9.307;
    m_inputDiffusion1 = 0.750;
    m_inputDiffusion2 = 0.625;
    
    m_pDecayDiffusionL1_delay = 22.580;
    m_pDecayDiffusionL2_delay = 60.482;
    m_pDecayDiffusionR1_delay = 30.510;
    m_pDecayDiffusionR2_delay = 89.244;
    m_decayDiffusion1 = 0.70;
    m_decayDiffusion2 = 0.15;
    
    m_damping = 0.05;
    /*
    m_pInputDiffusionA = new DiffusionTank(m_pInputDiffusionA_delay, m_inputDiffusion1, true, 1.0f, SamplingFrequency);
    m_pInputDiffusionB = new DiffusionTank(m_pInputDiffusionB_delay, m_inputDiffusion1, true, 1.0f, SamplingFrequency);
    m_pInputDiffusionC = new DiffusionTank(m_pInputDiffusionC_delay, m_inputDiffusion2, true, 1.0f, SamplingFrequency);
    m_pInputDiffusionD = new DiffusionTank(m_pInputDiffusionD_delay, m_inputDiffusion2, true, 1.0f, SamplingFrequency);
    
    m_pDecayDiffusionL1 = new DiffusionTank(m_pDecayDiffusionL1_delay, m_decayDiffusion1, false, 1.0f, SamplingFrequency);
    m_pDecayDiffusionL2 = new DiffusionTank(m_pDecayDiffusionL2_delay, m_decayDiffusion2, true, 1.0f, SamplingFrequency);
    m_pDecayDiffusionR1 = new DiffusionTank(m_pDecayDiffusionR1_delay, m_decayDiffusion1, false, 1.0f, SamplingFrequency);
    m_pDecayDiffusionR2 = new DiffusionTank(m_pDecayDiffusionR2_delay, m_decayDiffusion2, true, 1.0f, SamplingFrequency);
     */
    
    m_pInputDiffusionA = new DiffusionTankNode(m_pInputDiffusionA_delay, m_inputDiffusion1, true, 1.0f);
    m_pInputDiffusionB = new DiffusionTankNode(m_pInputDiffusionB_delay, m_inputDiffusion1, true, 1.0f);
    m_pInputDiffusionC = new DiffusionTankNode(m_pInputDiffusionC_delay, m_inputDiffusion2, true, 1.0f);
    m_pInputDiffusionD = new DiffusionTankNode(m_pInputDiffusionD_delay, m_inputDiffusion2, true, 1.0f);
    
    m_pDecayDiffusionL1 = new DiffusionTankNode(m_pDecayDiffusionL1_delay, m_decayDiffusion1, false, 1.0f);
    m_pDecayDiffusionL2 = new DiffusionTankNode(m_pDecayDiffusionL2_delay, m_decayDiffusion2, true, 1.0f);
    m_pDecayDiffusionR1 = new DiffusionTankNode(m_pDecayDiffusionR1_delay, m_decayDiffusion1, false, 1.0f);
    m_pDecayDiffusionR2 = new DiffusionTankNode(m_pDecayDiffusionR2_delay, m_decayDiffusion2, true, 1.0f);
    
    m_pBandWidthFilter = new FirstOrderFilterNode(1.0 - m_currentBandwidth, 1.0f);
    m_pDampingFilterL = new FirstOrderFilterNode(m_damping, 1.0f);
    m_pDampingFilterR = new FirstOrderFilterNode(m_damping, 1.0f);
    
    m_PreDelayLine_delay = 2;
    m_DelayLineL1_delay = 149.625;
    m_DelayLineL2_delay = 124.996;
    m_DelayLineR1_delay = 141.696;
    m_DelayLineR2_delay = 106.280;
    m_PreDelayLine_l = (int)(((double)m_PreDelayLine_delay / 1000.f) * m_SampleRate);
    m_DelayLineL1_l = (int)(((double)m_DelayLineL1_delay / 1000.f) * m_SampleRate);
    m_DelayLineL2_l = (int)(((double)m_DelayLineL2_delay / 1000.f) * m_SampleRate);
    m_DelayLineR1_l = (int)(((double)m_DelayLineR1_delay / 1000.f) * m_SampleRate);
    m_DelayLineR2_l = (int)(((double)m_DelayLineR2_delay / 1000.f) * m_SampleRate);
    m_DelayLine_c = 0;
    m_DelayLine_n = (int)(((double) DELAY_EFFECT_MAX_MS_DELAY / 1000.0f) * m_SampleRate);
    m_pPreDelayLine = new double[m_DelayLine_n];
    m_pDelayLineL1 = new double[m_DelayLine_n];
    m_pDelayLineL2 = new double[m_DelayLine_n];
    m_pDelayLineR1 = new double[m_DelayLine_n];
    m_pDelayLineR2 = new double[m_DelayLine_n];
    memset(m_pPreDelayLine, 0, sizeof(double) * m_DelayLine_n);
    memset(m_pDelayLineL1, 0, sizeof(double) * m_DelayLine_n);
    memset(m_pDelayLineL2, 0, sizeof(double) * m_DelayLine_n);
    memset(m_pDelayLineR1, 0, sizeof(double) * m_DelayLine_n);
    memset(m_pDelayLineR2, 0, sizeof(double) * m_DelayLine_n);
}

// Passes the AudioNode::GetNextSample to InputSample which calculates the next output sample
// and returns it
float ReverbNode::GetNextSample(unsigned long int timestamp) {
    float inVal = AudioNode::GetNextSample(timestamp);  // will get the input from all incoming nodes
    return InputSample(inVal);
}

inline double ReverbNode::InputSample(double sample) {
    double retVal = 0;
    
    if (sample > max)
        max = sample;
    else if (sample < min)
        min = sample;
    
    if(m_fPassThrough)
        return sample;
    
    // predelay
    long preDelaySampleIndex = m_DelayLine_c - m_PreDelayLine_l;
    if(preDelaySampleIndex < 0)
        preDelaySampleIndex = preDelaySampleIndex + m_DelayLine_n;
    double tempSample = m_pPreDelayLine[preDelaySampleIndex];
    m_pPreDelayLine[m_DelayLine_c] = sample/2;
    
    // Bandwidth filter
    tempSample = m_pBandWidthFilter->InputSample(m_currentBandwidth * tempSample);
    
    // cascade of input diffusers
    tempSample = m_pInputDiffusionA->InputSample(tempSample);
    tempSample = m_pInputDiffusionB->InputSample(tempSample);
    tempSample = m_pInputDiffusionC->InputSample(tempSample);
    double diffusionCascadeOut = m_pInputDiffusionD->InputSample(tempSample);
    
    // Reverb Tank (left side)
    long delayLineR2Index = m_DelayLine_c - m_DelayLineR2_l;
    if(delayLineR2Index < 0)
        delayLineR2Index = delayLineR2Index + m_DelayLine_n;
    double delayLineR2_out = m_pDelayLineR2[delayLineR2Index];
    
    tempSample = m_pDecayDiffusionL1->InputSample(diffusionCascadeOut + m_currentDecay * delayLineR2_out);
    
    m_pDelayLineL1[m_DelayLine_c] = tempSample;
    long delayLineL1Index = m_DelayLine_c - m_DelayLineL1_l;
    if(delayLineL1Index < 0)
        delayLineL1Index = delayLineL1Index + m_DelayLine_n;
    double delayLineL1_out = m_pDelayLineL1[delayLineL1Index];
    tempSample = m_pDampingFilterL->InputSample((1.0f - m_damping) * delayLineL1_out);
    tempSample = m_pDecayDiffusionL2->InputSample(m_currentDecay * tempSample);
    
    m_pDelayLineL2[m_DelayLine_c] = tempSample;
    long delayLineL2Index = m_DelayLine_c - m_DelayLineL2_l;
    if(delayLineL2Index < 0)
        delayLineL2Index = delayLineL2Index + m_DelayLine_n;
    double delayLineL2_out = m_pDelayLineL2[delayLineL2Index];
    
    // Reverb Tank (right side)
    tempSample = m_pDecayDiffusionR1->InputSample(diffusionCascadeOut + m_currentDecay * delayLineL2_out);
    
    m_pDelayLineR1[m_DelayLine_c] = tempSample;
    long delayLineR1Index = m_DelayLine_c - m_DelayLineR1_l;
    if(delayLineR1Index < 0)
        delayLineR1Index = delayLineR1Index + m_DelayLine_n;
    double delayLineR1_out = m_pDelayLineR1[delayLineR1Index];
    tempSample = m_pDampingFilterR->InputSample((1.0f - m_damping) * delayLineR1_out);
    tempSample = m_pDecayDiffusionR2->InputSample(m_currentDecay * tempSample);
    
    m_pDelayLineR2[m_DelayLine_c] = tempSample;
    
    // Modulation of the output tap location
    static long thetaN = 0;
    double modulation = sin(2.0f * M_PI * m_pLFOFreq->getValue() * (double)(thetaN / m_SampleRate));
    int excursion = (int)(modulation * m_pExcursion->getValue());
    // increment theta
    thetaN += 1;
    if(thetaN == m_SampleRate)
        thetaN = 0;
    
    // Output taps
    int tapIndex1 = 453 + excursion;
    int tapIndex2 = delayLineR1Index + 394 + excursion;
    int tapIndex3 = delayLineR1Index + 4407;
    int tapIndex4 = 2835 + excursion;
    int tapIndex5 = delayLineR2Index + 2958;
    int tapIndex6 = delayLineL1Index + 2949 + excursion;
    int tapIndex7 = 277;
    int tapIndex8 = delayLineL2Index + 158;
    
    if (tapIndex2 >= m_DelayLine_n)
        tapIndex2 -= m_DelayLine_n;
    if (tapIndex3 >= m_DelayLine_n)
        tapIndex3 -= m_DelayLine_n;
    if (tapIndex5 >= m_DelayLine_n)
        tapIndex5 -= m_DelayLine_n;
    if (tapIndex6 >= m_DelayLine_n)
        tapIndex6 -= m_DelayLine_n;
    if (tapIndex8 >= m_DelayLine_n)
        tapIndex8 -= m_DelayLine_n;
    
    double outL = 0.6 * m_pDecayDiffusionL1->GetSample(tapIndex1);
    outL += 0.6 * m_pDelayLineR1[tapIndex2];
    outL += 0.6 * m_pDelayLineR1[tapIndex3];
    outL -= 0.6 * m_pDecayDiffusionR2->GetSample(tapIndex4);
    outL += 0.6 * m_pDelayLineR2[tapIndex5];
    outL -= 0.6 * m_pDelayLineL1[tapIndex6];
    outL -= 0.6 * m_pDecayDiffusionL2->GetSample(tapIndex7);
    outL -= 0.6 * m_pDelayLineL2[tapIndex8];
    
    tapIndex1 = 782 + excursion;
    tapIndex2 = delayLineL1Index + 523;
    tapIndex3 = delayLineL1Index + 5374 + excursion;
    tapIndex4 = 1820;
    tapIndex5 = delayLineL2Index + 3961 + excursion;
    tapIndex6 = delayLineR1Index + 3128;
    tapIndex7 = 496;
    tapIndex8 = delayLineR2Index + 179 + excursion;
    
    if (tapIndex2 >= m_DelayLine_n)
        tapIndex2 -= m_DelayLine_n;
    if (tapIndex3 >= m_DelayLine_n)
        tapIndex3 -= m_DelayLine_n;
    if (tapIndex5 >= m_DelayLine_n)
        tapIndex5 -= m_DelayLine_n;
    if (tapIndex6 >= m_DelayLine_n)
        tapIndex6 -= m_DelayLine_n;
    if (tapIndex8 >= m_DelayLine_n)
        tapIndex8 -= m_DelayLine_n;
    
    double outR = 0.6 * m_pDecayDiffusionR1->GetSample(tapIndex1);
    outR += 0.6 * m_pDelayLineL1[tapIndex2];
    outR += 0.6 * m_pDelayLineL1[tapIndex3];
    outR -= 0.6 * m_pDecayDiffusionL2->GetSample(tapIndex4);
    outR += 0.6 * m_pDelayLineL2[tapIndex5];
    outR -= 0.6 * m_pDelayLineR1[tapIndex6];
    outR -= 0.6 * m_pDecayDiffusionR2->GetSample(tapIndex7);
    outR -= 0.6 * m_pDelayLineR2[tapIndex8];
    
    double reverb = outL + outR;
    
    retVal = (1-m_pWet->getValue())*sample + m_pWet->getValue() * reverb;
    
    m_DelayLine_c++;
    if(m_DelayLine_c >= m_DelayLine_n)
        m_DelayLine_c = 0;
    
    return retVal;
}

void ReverbNode::Reset() {
    EffectNode::Reset();
    
    m_pInputDiffusionA->ClearOutEffect();
    m_pInputDiffusionB->ClearOutEffect();
    m_pInputDiffusionC->ClearOutEffect();
    m_pInputDiffusionD->ClearOutEffect();
    m_pDecayDiffusionL1->ClearOutEffect();
    m_pDecayDiffusionL2->ClearOutEffect();
    m_pDecayDiffusionR1->ClearOutEffect();
    m_pDecayDiffusionR2->ClearOutEffect();
    
    ClearOutEffect();
    
    // reset to default values
    SetDecay(0.15);
    SetBandwidth(0.75);
}

void ReverbNode::ClearOutEffect() {
    memset(m_pPreDelayLine, 0, sizeof(double) * m_DelayLine_n);
    memset(m_pDelayLineL1, 0, sizeof(double) * m_DelayLine_n);
    memset(m_pDelayLineL2, 0, sizeof(double) * m_DelayLine_n);
    memset(m_pDelayLineR1, 0, sizeof(double) * m_DelayLine_n);
    memset(m_pDelayLineR2, 0, sizeof(double) * m_DelayLine_n);
}

bool ReverbNode::SetBandwidth(double bandwidth) {
    if (!m_pBandwidth->setValue(bandwidth))
        return false;

    m_currentBandwidth = m_pBandwidth->getValue();
    m_pBandWidthFilter->SetFeedback(1.0f - m_currentBandwidth);
    return true;
}

bool ReverbNode::SetDamping(double damping) {
    if (damping > 1.0 || damping < 0.0)
        return false;

    m_damping = damping;
    m_pDampingFilterL->SetFeedback(m_damping);
    m_pDampingFilterR->SetFeedback(m_damping);
    return true;
}

bool ReverbNode::SetDecay(double decay) {
    if (!m_pDecay->setValue(decay))
        return false;

    m_currentDecay = m_pDecay->getValue();
    return true;
}

bool ReverbNode::SetInputDiffusion1(double diffusion) {
    if (diffusion > 1.0 || diffusion < 0.0)
        return false;

    m_inputDiffusion1 = diffusion;
    m_pInputDiffusionA->SetFeedback(m_inputDiffusion1);
    m_pInputDiffusionB->SetFeedback(m_inputDiffusion1);
    return true;
}

bool ReverbNode::SetInputDiffusion2(double diffusion) {
    if (diffusion > 1.0 || diffusion < 0.0)
        return false;

    m_inputDiffusion2 = diffusion;
    m_pInputDiffusionC->SetFeedback(m_inputDiffusion2);
    m_pInputDiffusionD->SetFeedback(m_inputDiffusion2);
    return true;
}

bool ReverbNode::SetDecayDiffusion1(double diffusion) {
    if (diffusion > 1.0 || diffusion < 0.0)
        return false;

    m_decayDiffusion1 = diffusion;
    m_pDecayDiffusionL1->SetFeedback(m_decayDiffusion1);
    m_pDecayDiffusionR1->SetFeedback(m_decayDiffusion1);
    return true;
}

bool ReverbNode::SetDecayDiffusion2(double diffusion) {
    if (diffusion > 1.0 || diffusion < 0.0)
        return false;

    m_decayDiffusion2 = diffusion;
    m_pDecayDiffusionL2->SetFeedback(m_decayDiffusion2);
    m_pDecayDiffusionR2->SetFeedback(m_decayDiffusion2);
    return true;
}

bool ReverbNode::SetPreDelayLength(double scale) {
    m_PreDelayLine_l = (int)scale;
    return true;
}

bool ReverbNode::SetDelayL1Length(double length) {
    m_DelayLineL1_l = (int)length;
    return true;
}

bool ReverbNode::SetDelayL2Length(double length) {
    m_DelayLineL2_l = (int)length;
    return true;
}

bool ReverbNode::SetDelayR1Length(double length) {
    m_DelayLineR1_l = (int)length;
    return true;
}

bool ReverbNode::SetDelayR2Length(double length) {
    m_DelayLineR2_l = (int)length;
    return true;
}

Parameter* ReverbNode::getPrimaryParam() {
    return m_pDecay;
}

bool ReverbNode::setPrimaryParam(float value) {
    return SetDecay(value);
}

Parameter* ReverbNode::getSecondaryParam() {
    return m_pBandwidth;
}

bool ReverbNode::setSecondaryParam(float value) {
    return SetBandwidth(value);
}

Parameter* ReverbNode::getLFO() {
    return m_pLFOFreq;
}

Parameter* ReverbNode::getExcursion() {
    return m_pExcursion;
}

ReverbNode::~ReverbNode() {
    if(m_pInputDiffusionA != NULL) {
        delete m_pInputDiffusionA;
        m_pInputDiffusionA = NULL;
    }
    
    if(m_pInputDiffusionB != NULL) {
        delete m_pInputDiffusionB;
        m_pInputDiffusionB = NULL;
    }
    
    if(m_pInputDiffusionC != NULL) {
        delete m_pInputDiffusionC;
        m_pInputDiffusionC = NULL;
    }
    
    if(m_pInputDiffusionD != NULL) {
        delete m_pInputDiffusionD;
        m_pInputDiffusionA = NULL;
    }
    
    if(m_pDecayDiffusionL1 != NULL) {
        delete m_pDecayDiffusionL1;
        m_pDecayDiffusionL1 = NULL;
    }
    
    if(m_pDecayDiffusionL2 != NULL) {
        delete m_pDecayDiffusionL2;
        m_pDecayDiffusionL2 = NULL;
    }
    
    if(m_pDecayDiffusionR1 != NULL) {
        delete m_pDecayDiffusionR1;
        m_pDecayDiffusionR1 = NULL;
    }
    
    if(m_pDecayDiffusionR2) {
        delete m_pDecayDiffusionR2;
        m_pDecayDiffusionR2 = NULL;
    }
    
    if(m_pPreDelayLine != NULL) {
        delete m_pPreDelayLine;
        m_pPreDelayLine = NULL;
    }
    
    if(m_pDelayLineL1 != NULL) {
        delete m_pDelayLineL1;
        m_pDelayLineL1 = NULL;
    }
    
    if(m_pDelayLineL2 != NULL) {
        delete m_pDelayLineL2;
        m_pDelayLineL2 = NULL;
    }
    
    if(m_pDelayLineR1 != NULL) {
        delete m_pDelayLineR1;
        m_pDelayLineR1 = NULL;
    }
    
    if(m_pDelayLineR2 != NULL) {
        delete m_pDelayLineR2;
        m_pDelayLineR2 = NULL;
    }
    
    if(m_pLFOFreq != NULL) {
        delete m_pLFOFreq;
        m_pLFOFreq = NULL;
    }
    
    if(m_pExcursion != NULL) {
        delete m_pExcursion;
        m_pExcursion = NULL;
    }
    
    if(m_pDecay != NULL) {
        delete m_pDecay;
        m_pDecay = NULL;
    }
    
    if(m_pBandwidth != NULL) {
        delete m_pBandwidth;
        m_pBandwidth = NULL;
    }
    
    if(m_pBandWidthFilter != NULL) {
        delete m_pBandWidthFilter;
        m_pBandWidthFilter = NULL;
    }
    
    if(m_pDampingFilterL != NULL) {
        delete m_pDampingFilterL;
        m_pDampingFilterL = NULL;
    }
    
    if(m_pDampingFilterR != NULL) {
        delete m_pDampingFilterR;
        m_pDampingFilterR = NULL;
    }
}



