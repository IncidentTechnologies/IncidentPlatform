#pragma once

// XMPAttribute is an attribute within an XMP Tag 

#include "dss_list.h"
#include "SmartBuffer.h"
#include "XMPValue.h"
#include "XMPElement.h"

using namespace dss;

class XMPAttribute {
public:
    XMPAttribute(char *pszName = NULL, char *pszValue = NULL);
    XMPAttribute(char *pszName = NULL, int value = 0);
    XMPAttribute(char *pszName = NULL, long int value = 0);
    XMPAttribute(char *pszName = NULL, double value = 0.0f);
    XMPAttribute(XMPElement *pElement);

    char *GetName();
    XMPValue GetXMPValue();

private:
    char *m_pszName;
    XMPValue m_XMPValue;
};


