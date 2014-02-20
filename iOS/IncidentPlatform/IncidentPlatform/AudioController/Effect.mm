#include "Effect.h"
#include <string.h>

Effect::Effect(std::string name, double wet, double SamplingFrequency) :
    m_name(name),
    m_fPassThrough(false),
    m_SamplingFrequency(SamplingFrequency)
{
    m_pWet = new Parameter(wet, 0.0, 1.0, "Wet");
}

bool Effect::SetPassThru(bool state)
{
    m_fPassThrough = state;
    if (state)
    {
        ClearOutEffect();
    }
    return true;
}

bool Effect::SetWet(double wet)
{
    return m_pWet->setValue(wet);
}

float Effect::GetWet()
{
    return m_pWet->getValue();
}

void Effect::Reset()
{
    m_fPassThrough = true;
}

void Effect::ClearOutEffect()
{
    
}

bool Effect::isOn()
{
    return !m_fPassThrough;
}

Effect::~Effect()
{
    
}

std::string Effect::getName() {return m_name;};
Parameter& Effect::getPrimaryParam() {/*empty default*/ Parameter *p = new Parameter(0, 0, 0, NULL); return *p; };
bool Effect::setPrimaryParam(float value) {/*empty default*/ return false; };
Parameter& Effect::getSecondaryParam() {/*empty default*/ Parameter *p = new Parameter(0, 0, 0, NULL); return *p; };
bool Effect::setSecondaryParam(float value) {/*empty default*/ return false; };
