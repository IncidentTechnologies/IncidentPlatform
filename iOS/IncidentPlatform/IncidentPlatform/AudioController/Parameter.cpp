#include "Parameter.h"

Parameter::Parameter(float initial, float min, float max, const std::string& desc)
{
    m_max = max;
    m_min = min;
    
    if (initial > max)
        m_value = max;
    else if (initial < min)
        m_value = min;
    else
        m_value = initial;

    m_name = desc;
}
    
float Parameter::getValue() {
    return m_value;
}

bool Parameter::setValue(float newVal)
{
    if (newVal > m_max || newVal < m_min)
        return false;

    m_value = newVal;
    return true;
}
    
float Parameter::getMax() {return m_max;};
float Parameter::getMin() {return m_min;};

std::string Parameter::getName() {
    return m_name;
}

