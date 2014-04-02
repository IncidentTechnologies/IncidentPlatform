#include "XMPValue.h"
#include <string>
#include <stdlib.h>
#include "SmartBuffer.h"
#include "XMPNode.h"

XMPValue::XMPValue() :
    m_ValueType(XMP_VALUE_INVALID),
    m_Buffer(NULL),
    m_BufferSize(0),
    m_pszName(NULL)
{
    // empty
}

XMPValue::XMPValue(XMPNode *xmpNode) :
    m_ValueType(XMP_VALUE_INVALID),
    m_Buffer(NULL),
    m_BufferSize(0),
    m_pszName(NULL)
{
    if(xmpNode->HasContent()) {
        m_pszName = (char*)malloc(strlen(xmpNode->GetName() + 1));
        strcpy(m_pszName, xmpNode->GetName());
        SetValue(xmpNode->text());
    }
}

XMPValue::XMPValue(char *pszValue) :
    m_ValueType(XMP_VALUE_INVALID),
    m_Buffer(NULL),
    m_BufferSize(0),
    m_pszName(NULL)
{
    SetValue(pszValue);
}

XMPValue::XMPValue(int value) :
    m_ValueType(XMP_VALUE_INVALID),
    m_Buffer(NULL),
    m_BufferSize(0),
    m_pszName(NULL)
{
    SetValueInt((long int)value);
}

XMPValue::XMPValue(long int value) :
    m_ValueType(XMP_VALUE_INVALID),
    m_Buffer(NULL),
    m_BufferSize(0),
    m_pszName(NULL)
{
    SetValueInt(value);
}

XMPValue::XMPValue(double value) :
    m_ValueType(XMP_VALUE_INVALID),
    m_Buffer(NULL),
    m_BufferSize(0),
    m_pszName(NULL)
{
    SetValueDouble(value);
}

RESULT XMPValue::SetValueInt(long int value) {
    RESULT r = R_SUCCESS;

    // This is an integer
    m_ValueType = XMP_VALUE_INTEGER;
    m_Buffer = new long int;

    memcpy(m_Buffer, (void*)(&value), sizeof(long int));
    m_BufferSize = sizeof(long int);

Error:
    return r;
}

RESULT XMPValue::SetValueDouble(double value) {
    RESULT r = R_SUCCESS;
    
    // This is an integer
    m_ValueType = XMP_VALUE_DOUBLE;
    m_Buffer = new double;
    
    memcpy(m_Buffer, (void*)(&value), sizeof(double));
    m_BufferSize = sizeof(double);
    
Error:
    return r;
}

RESULT XMPValue::SetValueChar(char value) {
    RESULT r = R_SUCCESS;
    
    // This is an integer
    m_ValueType = XMP_VALUE_CHAR;
    m_Buffer = new char;
    
    memcpy(m_Buffer, (void*)(&value), sizeof(char));
    m_BufferSize = sizeof(char);
    
Error:
    return r;
}

RESULT XMPValue::GetValueInt(long int *value) {
    if(this->m_ValueType == XMP_VALUE_DOUBLE) {
        *value = (long int)(*((double*)m_Buffer));
        return R_SUCCESS;
    }
    else if(this->m_ValueType == XMP_VALUE_INTEGER) {
        *value = *((long int*)m_Buffer);
        return R_SUCCESS;
    }
    else if(this->m_ValueType == XMP_VALUE_STRING) {
        *value = atol((char *)m_Buffer);
        return R_SUCCESS;
    }
    else {
        return R_FAIL;
    }
}

RESULT XMPValue::GetValueDouble(double *value) {
    if(this->m_ValueType == XMP_VALUE_DOUBLE) {
        *value = *((double*)m_Buffer);
        return R_SUCCESS;
    }
    else if(this->m_ValueType == XMP_VALUE_INTEGER) {
        *value = (double)(*((long int*)m_Buffer));
        return R_SUCCESS;
    }
    else if(this->m_ValueType == XMP_VALUE_STRING) {
        *value = atof((char *)m_Buffer);
        return R_SUCCESS;
    }
    else {
        return R_FAIL;
    }
}

RESULT XMPValue::GetValueBool(bool *value) {
    if(this->m_ValueType == XMP_VALUE_DOUBLE) {
        if(*((double*)m_Buffer) == 0)
            *value = false;
        else
            *value = true;
        
        return R_SUCCESS;
    }
    else if(this->m_ValueType == XMP_VALUE_INTEGER) {
        if((*((long int*)m_Buffer)) == 0)
            *value = false;
        else
            *value = true;
        
        return R_SUCCESS;
    }
    else if(this->m_ValueType == XMP_VALUE_STRING) {
        if(strcmp((char *)m_Buffer, "true") == 0 || strcmp((char *)m_Buffer, "TRUE") == 0)
            *value = true;
        else
            *value = false;
        
        return R_SUCCESS;
    }
    else {
        return R_FAIL;
    }
}

char* XMPValue::GetPszValue() {
    char *ReturnBuffer = new char[25];
    memset(ReturnBuffer, 0, sizeof(char) * 25);

    switch(m_ValueType)
    {
        case XMP_VALUE_INTEGER: sprintf(ReturnBuffer, "%d", *((long int*)m_Buffer));
                                break;
        case XMP_VALUE_DOUBLE:  sprintf(ReturnBuffer, "%f", *((double*)m_Buffer));
                                break;
        case XMP_VALUE_CHAR:    sprintf(ReturnBuffer, "%c", *((char*)m_Buffer));
                                break;
        case XMP_VALUE_STRING:  sprintf(ReturnBuffer, "%s", (char*)m_Buffer);
                                break;
        case XMP_VALUE_INVALID: 
        default:                strcpy(ReturnBuffer, "InvalidValue");
                                break;
    }
    
    return ReturnBuffer;
}

RESULT XMPValue::SetValueType(XMP_VALUE_TYPE xvt){ m_ValueType = xvt; return R_SUCCESS; }

RESULT XMPValue::SetValue(char *pszValue) {
    RESULT r = R_SUCCESS;

    // Scan the string and determine what kind of value it is
    int Length = strlen(pszValue);
    bool ContainsChars = false;
    bool FoundDecimalPoint = false;

    for(int i = 0; i < Length; i++)        
        if(pszValue[i] < '0' || pszValue[i] > '9')
            ContainsChars = true;
    
    if(!ContainsChars)
        for(int i = 0; i < Length; i++)        
            if(pszValue[i] == '.') {
                if(FoundDecimalPoint = false) 
                    FoundDecimalPoint = true;
                else
                    ContainsChars = true;       // found two decimal points                
            }

    if(ContainsChars) {
        if(Length > 1) {
            // Must be a string
            m_ValueType = XMP_VALUE_STRING;
            m_Buffer = new char[Length];
            strcpy((char*)m_Buffer, pszValue);
            m_BufferSize = sizeof(char) * Length;
        }
        else if(Length == 0) {
            m_ValueType = XMP_VALUE_CHAR;
            m_Buffer = new char;
            memcpy(m_Buffer, pszValue, sizeof(char));
            m_BufferSize = sizeof(char);
        }
    }
    else {
        if(FoundDecimalPoint) {
            // This is a floating point number
            m_ValueType = XMP_VALUE_DOUBLE;
            m_Buffer = new double;
            char *pEnd;
            float *TempFloat = new float(strtod(pszValue, &pEnd));
            memcpy(m_Buffer, (void*)TempFloat, sizeof(double));
            m_BufferSize = sizeof(double);
        }
        else {
            // This is an integer
            m_ValueType = XMP_VALUE_INTEGER;
            m_Buffer = new long int;
            char *pEnd;
            long int *TempInt = new long int(strtol(pszValue, &pEnd, 10));                
            memcpy(m_Buffer, (void*)TempInt, sizeof(long int));
            m_BufferSize = sizeof(long int);
        }
    }

Error:
    return r;
}

RESULT XMPValue::SetBuffer(void *newBuffer, int newBuffer_n, XMP_VALUE_TYPE newType) {
    RESULT r = R_SUCCESS;
    
    if(m_Buffer != NULL) {
        delete m_Buffer;
        m_Buffer = NULL;
    }
    
    m_ValueType = newType;
    m_BufferSize = newBuffer_n;
    m_Buffer = (void*)malloc(m_BufferSize);
    memcpy(m_Buffer, newBuffer, m_BufferSize);
    
Error:
    return r;
}

// Do it statically
SmartBuffer XMPValue::CreateSmartBuffer(XMPValue *xmpValue) {
    SmartBuffer retBuf(xmpValue->GetPszValue());
    return retBuf;
}

// Operators
// TODO: Plenty of code-reuse clean up here!

XMPValue & XMPValue::operator=(const XMPValue &rhs) {
    if(this != &rhs)
        this->SetBuffer(rhs.m_Buffer, rhs.m_BufferSize, rhs.m_ValueType);
    
    return *this;
}

XMPValue & XMPValue::operator+=(const XMPValue &rhs) {
    switch(this->m_ValueType) {
        case XMP_VALUE_STRING: {
            
            // If value is a string then just convert into SB and do it that way
            SmartBuffer sb_temp = CreateSmartBuffer(this) + CreateSmartBuffer((XMPValue*)(&rhs));
            this->SetBuffer((void*)(sb_temp.GetBuffer()), sb_temp.GetBufferLength(), XMP_VALUE_STRING);
            
            return *this;
        } break;
            

        case XMP_VALUE_INTEGER: {
            long int val = *((long int*)this->m_Buffer);
            
            switch (rhs.m_ValueType) {
                case XMP_VALUE_DOUBLE: { val += *((double*)this->m_Buffer); } break;
                case XMP_VALUE_INTEGER: { val += *((long int*)this->m_Buffer); } break;
                case XMP_VALUE_CHAR: { val += *((char*)this->m_Buffer); } break;
                case XMP_VALUE_STRING: {
                    // TODO: int + string not working yet
                } break;
                
            }
            
            this->SetValueInt(val);
            return *this;
        } break;
            
        case XMP_VALUE_DOUBLE: {
            double val = *((double*)this->m_Buffer);
            
            switch (rhs.m_ValueType) {
                case XMP_VALUE_DOUBLE: { val += *((double*)this->m_Buffer); } break;
                case XMP_VALUE_INTEGER: { val += *((long int*)this->m_Buffer); } break;
                case XMP_VALUE_CHAR: { val += *((char*)this->m_Buffer); } break;
                case XMP_VALUE_STRING: {
                    // TODO: int + string not working yet
                } break;
                    
            }
            
            this->SetValueDouble(val);
            return *this;
        } break;
            
        case XMP_VALUE_CHAR: {
            // TODO: Not supported
            return *this;
        }
    }
    
    return *this;
}

XMPValue & XMPValue::operator--(int /*unused*/) {
    switch(this->m_ValueType) {
        case XMP_VALUE_STRING: {
            
            // If value is a string then just convert into SB and do it that way
            SmartBuffer sb_temp = CreateSmartBuffer(this)--;
            this->SetBuffer((void*)(sb_temp.GetBuffer()), sb_temp.GetBufferLength(), XMP_VALUE_STRING);
            
            return *this;
        } break;
            
            
        case XMP_VALUE_INTEGER: {
            this->SetValueInt((*((long int*)this->m_Buffer))--);
            return *this;
        } break;
            
        case XMP_VALUE_DOUBLE: {
            this->SetValueDouble((*((double*)this->m_Buffer))--);
            return *this;
        } break;
            
        case XMP_VALUE_CHAR: {
            this->SetValueChar((*((char*)this->m_Buffer))--);
            return *this;
        }
    }
    
    return *this;
}

XMPValue & XMPValue::operator++(int /*unused*/) {
    switch(this->m_ValueType) {
        case XMP_VALUE_STRING: {
            // TODO: Increment on string unsupported
            return *this;
        } break;
            
            
        case XMP_VALUE_INTEGER: {
            this->SetValueInt((*((long int*)this->m_Buffer))++);
            return *this;
        } break;
            
        case XMP_VALUE_DOUBLE: {
            this->SetValueDouble((*((double*)this->m_Buffer))++);
            return *this;
        } break;
            
        case XMP_VALUE_CHAR: {
            this->SetValueChar((*((char*)this->m_Buffer))++);
            return *this;
        }
    }
    
    return *this;
}

XMPValue & XMPValue::operator-=(const XMPValue &rhs) {
    switch(this->m_ValueType) {
        case XMP_VALUE_STRING: {
            
            // If value is a string then just convert into SB and do it that way
            // TODO: subtraction on string un-supported
            //SmartBuffer sb_temp = CreateSmartBuffer(this) - CreateSmartBuffer((XMPValue*)(&rhs));
            //this->SetBuffer((void*)(sb_temp.GetBuffer()), sb_temp.GetBufferLength(), XMP_VALUE_STRING);
            
            return *this;
        } break;
            
            
        case XMP_VALUE_INTEGER: {
            long int val = *((long int*)this->m_Buffer);
            
            switch (rhs.m_ValueType) {
                case XMP_VALUE_DOUBLE: { val -= *((double*)this->m_Buffer); } break;
                case XMP_VALUE_INTEGER: { val -= *((long int*)this->m_Buffer); } break;
                case XMP_VALUE_CHAR: { val -= *((char*)this->m_Buffer); } break;
                case XMP_VALUE_STRING: {
                    // TODO: int + string not working yet
                } break;
                    
            }
            
            this->SetValueInt(val);
            return *this;
        } break;
            
        case XMP_VALUE_DOUBLE: {
            double val = *((double*)this->m_Buffer);
            
            switch (rhs.m_ValueType) {
                case XMP_VALUE_DOUBLE: { val -= *((double*)this->m_Buffer); } break;
                case XMP_VALUE_INTEGER: { val -= *((long int*)this->m_Buffer); } break;
                case XMP_VALUE_CHAR: { val -= *((char*)this->m_Buffer); } break;
                case XMP_VALUE_STRING: {
                    // TODO: int + string not working yet
                } break;
                    
            }
            
            this->SetValueDouble(val);
            return *this;
        } break;
            
        case XMP_VALUE_CHAR: {
            // TODO: Not supported
            return *this;
        }
    }
    
    return *this;
}

const XMPValue XMPValue::operator+(const XMPValue &rhs) {
    switch(m_ValueType) {
        case XMP_VALUE_STRING: {
            
            // If value is a string then just convert into SB and do it that way
            XMPValue temp;
            SmartBuffer sb_temp = CreateSmartBuffer(this) + CreateSmartBuffer((XMPValue*)(&rhs));
            temp.SetBuffer((void*)(sb_temp.GetBuffer()), sb_temp.GetBufferLength(), XMP_VALUE_STRING);
            
            return temp;
        } break;
            
            
        case XMP_VALUE_INTEGER: {
            long int val = *((long int*)m_Buffer);
            
            switch (rhs.m_ValueType) {
                case XMP_VALUE_DOUBLE: { val += *((double*)m_Buffer); } break;
                case XMP_VALUE_INTEGER: { val += *((long int*)m_Buffer); } break;
                case XMP_VALUE_CHAR: { val += *((char*)m_Buffer); } break;
                case XMP_VALUE_STRING: {
                    // TODO: int + string not working yet
                } break;
                    
            }
            
            return XMPValue(val);
        } break;
            
        case XMP_VALUE_DOUBLE: {
            double val = *((double*)m_Buffer);
            
            switch (rhs.m_ValueType) {
                case XMP_VALUE_DOUBLE: { val += *((double*)m_Buffer); } break;
                case XMP_VALUE_INTEGER: { val += *((long int*)m_Buffer); } break;
                case XMP_VALUE_CHAR: { val += *((char*)m_Buffer); } break;
                case XMP_VALUE_STRING: {
                    // TODO: int + string not working yet
                } break;
                    
            }
            
            return XMPValue(val);
        } break;
            
        case XMP_VALUE_CHAR: {
            // TODO: Not supported
            return XMPValue();
        }
    }
    
    return XMPValue();
}

const XMPValue XMPValue::operator-(const XMPValue &rhs){
    switch(m_ValueType) {
        case XMP_VALUE_STRING: {
            
            // If value is a string then just convert into SB and do it that way
            // TODO: subtraction on string un-supported
            //SmartBuffer sb_temp = CreateSmartBuffer(this) - CreateSmartBuffer((XMPValue*)(&rhs));
            //this->SetBuffer((void*)(sb_temp.GetBuffer()), sb_temp.GetBufferLength(), XMP_VALUE_STRING);
            
            return XMPValue();
        } break;
            
            
        case XMP_VALUE_INTEGER: {
            long int val = *((long int*)m_Buffer);
            
            switch (rhs.m_ValueType) {
                case XMP_VALUE_DOUBLE: { val -= *((double*)m_Buffer); } break;
                case XMP_VALUE_INTEGER: { val -= *((long int*)m_Buffer); } break;
                case XMP_VALUE_CHAR: { val -= *((char*)m_Buffer); } break;
                case XMP_VALUE_STRING: {
                    // TODO: int + string not working yet
                } break;
                    
            }
            
            return XMPValue(val);
        } break;
            
        case XMP_VALUE_DOUBLE: {
            double val = *((double*)m_Buffer);
            
            switch (rhs.m_ValueType) {
                case XMP_VALUE_DOUBLE: { val -= *((double*)m_Buffer); } break;
                case XMP_VALUE_INTEGER: { val -= *((long int*)m_Buffer); } break;
                case XMP_VALUE_CHAR: { val -= *((char*)m_Buffer); } break;
                case XMP_VALUE_STRING: {
                    // TODO: int + string not working yet
                } break;
                    
            }
            
            return XMPValue(val);
        } break;
            
        case XMP_VALUE_CHAR: {
            // TODO: Not supported
            return XMPValue();
        }
    }
    
    return XMPValue();
}

char* XMPValue::GetName() {
    return m_pszName;
}







