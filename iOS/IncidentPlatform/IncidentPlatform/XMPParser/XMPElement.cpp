#include "XMPElement.h"
#include <string>

const char* pszXMPElementType[] = { "Start Tag",
    "End Tag",
    "Declaration",
    "Empty Tag",
    "Comment Tag",
    "Content",
    "Tag Attribute",
    "Invalid Tag"
};

XMPElement::XMPElement(XMP_ELEMENT_TYPE type, char *pszName, char *pszValue) :
    m_Type(type),
    m_pszName(pszName),
    m_pszValue(pszValue),
    m_pBuffer(NULL),
    m_pBuffer_s(-1),
    m_pszDataname(NULL),
m_dataid(-1)
{
    /*empty here*/
}

XMPElement::XMPElement(void *pDataBuffer, long int datasize, int dataid, char *pszDataName) :
    m_Type(XMP_CONTENT_DATA),
    m_pszName(NULL),
    m_pszValue(NULL),
    m_pBuffer(pDataBuffer),
    m_pBuffer_s(datasize),
    m_pszDataname(pszDataName),
    m_dataid(dataid)
{
    /*empty here*/
}

XMPElement::~XMPElement() {
    /*empty stub*/
}

RESULT XMPElement::PrintElement() {
    printf("Element type:%s ", pszXMPElementType[m_Type]);
    
    if(m_pszName != NULL)
        printf("name:%s ", m_pszName);
    
    if(m_pszValue != NULL)
        printf("value:%s ", m_pszValue);
    
    printf("\n");
    return R_SUCCESS;
}

XMP_ELEMENT_TYPE XMPElement::GetElementType(){
    return m_Type;
}

RESULT XMPElement::SetElementType(XMP_ELEMENT_TYPE type){
    m_Type = type;
    return R_SUCCESS;
}

char* XMPElement::GetName(){
    return m_pszName;
}

char* XMPElement::GetValue(){
    return m_pszValue;
}

void* XMPElement::GetBuffer(){
    return m_pBuffer;
}

long int XMPElement::GetBufferSize(){
    return m_pBuffer_s;
}

int XMPElement::GetDataID(){
    return m_dataid;
}

char* XMPElement::GetDataName(){
    return m_pszDataname;
}

