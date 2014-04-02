#include "XMPAttribute.h"
#include <string>
#include <stdlib.h>

XMPAttribute::XMPAttribute(char *pszName, char *pszValue) :
  m_pszName(pszName),
  m_XMPValue(pszValue)
{      
    if(pszName == NULL)        
        m_XMPValue.SetValueType(XMP_VALUE_INVALID);        // post-facto invalidation if name is not present
}

XMPAttribute::XMPAttribute(char *pszName, int value) :
    m_pszName(pszName),
    m_XMPValue(value)
{
    if(pszName == NULL)
        m_XMPValue.SetValueType(XMP_VALUE_INVALID);        // post-facto invalidation if name is not present
}

XMPAttribute::XMPAttribute(char *pszName, long int value) :
    m_pszName(pszName),
    m_XMPValue(value)
{      
    if(pszName == NULL)        
        m_XMPValue.SetValueType(XMP_VALUE_INVALID);        // post-facto invalidation if name is not present
}

XMPAttribute::XMPAttribute(char *pszName, double value) :
    m_pszName(pszName),
    m_XMPValue(value)
{
    if(pszName == NULL)
        m_XMPValue.SetValueType(XMP_VALUE_INVALID);        // post-facto invalidation if name is not present
}

XMPAttribute::XMPAttribute(XMPElement *pElement) :
  m_pszName(pElement->GetName()),
  m_XMPValue(pElement->GetValue())
{
    if(m_pszName == NULL)
        m_XMPValue.SetValueType(XMP_VALUE_INVALID);         // post-facto invalidation if name is not present
}

char* XMPAttribute::GetName(){
  return m_pszName; 
}

XMPValue XMPAttribute::GetXMPValue(){
  return m_XMPValue; 
}
