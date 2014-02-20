#pragma once

#include "EHM.h"

using namespace dss;

typedef enum XMPElementType
{
    XMP_START_TAG,      // <tag>
    XMP_END_TAG,        // </tag>
    XMP_DECLARATION,    // <?declaration>     NOT SUPPORTED FOR NOW
    XMP_EMPTY,          // <empty/>
    XMP_COMMENT,        // <!-- comment -->  (usually not processed beyond this stage)   NOT SUPPORTED FOR NOW!
    XMP_CONTENT,        // this is not a tag but the content between two tags
    XMP_TAG_ATTRIBUTE,      // Attribute inside of a tag
    XMP_CONTENT_DATA,   // XMP specific data content
    XMP_INVALID         // an invalid XMP type, error!
} XMP_ELEMENT_TYPE;

extern const char* pszXMPElementType[];

class XMPElement {
public:
    XMPElement(XMP_ELEMENT_TYPE type, char *pszName, char *pszValue = NULL);
    XMPElement(void *pDataBuffer, long int datasize, int dataid, char *pszDataName);
    ~XMPElement();
    
    RESULT PrintElement();
    XMP_ELEMENT_TYPE GetElementType();
    RESULT SetElementType(XMP_ELEMENT_TYPE type);
    
    char *GetName();
    char *GetValue();
    void *GetBuffer();
    long int GetBufferSize();
    int GetDataID();
    char *GetDataName();
    
private:
    XMP_ELEMENT_TYPE m_Type;
    char *m_pszName;
    char *m_pszValue;
    
    // data info
    void *m_pBuffer;
    long int m_pBuffer_s;
    int m_dataid;
    char *m_pszDataname;
};
