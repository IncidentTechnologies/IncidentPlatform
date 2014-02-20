#ifndef _SMARTBUFFER_H
#define _SMARTBUFFER_H
#pragma once

#define SMART_BUFFER_BLOCK_SIZE 256

#include "RESULT.h"
#include "EHM.h"
#include "dss_list.h"

namespace dss {

class SmartBuffer
{
public:
    SmartBuffer(const char *pBuffer = NULL, int pBuffer_n = 0);
    SmartBuffer(SmartBuffer *psb);
    ~SmartBuffer();

    RESULT ResetBuffer(const char *pszBuffer);
    RESULT ResetBuffer(const char *pBuffer, int pBuffer_n);
    RESULT ResetBuffer();
    RESULT Reset();
    RESULT reset();

    // Buffer Tools
    char *CreateBufferCopy();
    bool NotAllWhitespace();

    // Buffer Manipulation
    RESULT IncrementBufferSize();
    RESULT Append(char byte);
    RESULT Append(SmartBuffer sb);

    RESULT RemoveCharacter(char *pChar);    // Removes character at pointer, pointer should point inside the bounds of m_Buffer

    RESULT Find(const char byte, char*&r_pChar);            // will return a pointer into the buffer of the first found byte
    char *Find(const SmartBuffer *psb);                     // Will return a pointer into the buffer of the first found instance of psb
 
    // Operators
    SmartBuffer &operator=(const SmartBuffer &rhs);     // Equality (sets buffer)
    SmartBuffer &operator+=(char byte);                 // Appends a character to the buffer
    SmartBuffer &operator+=(const char *psz);                 // Appends a null terminated string to the buffer
    SmartBuffer &operator+=(const SmartBuffer &rhs);    // Appends a buffer
    SmartBuffer &operator--(int /*unused*/);            // removes last character    
    SmartBuffer &operator-=(int n);                     // Removes n characters from end
    SmartBuffer &operator-=(const char byte);           // Removes first instance of byte from the string
    SmartBuffer &operator%=(const char byte);           // Removes ALL instances of byte from the string
    
    const SmartBuffer operator+(const char byte);       
    const SmartBuffer operator+(const char *psz);       // Add a null terminated string to the buffer
    const SmartBuffer operator+(const SmartBuffer &rhs);
    const SmartBuffer operator-(const int n);                 // Will remove the n last characters from the string
    const SmartBuffer operator-(const char byte);           // this will remove first instance of byte in buffer
    //const SmartBuffer operator-(const SmartBuffer &rhs);    // This will remove the first instance of rhs found in the buffer
    const SmartBuffer operator%(const char byte);           // This will remove all instances of byte found in the buffer
    //const SmartBuffer operator%(const SmartBuffer &rhs);    // This will remove all instances of rhs found in the buffer
    
    char* GetBuffer();
    int GetBufferLength();
    int Length();
    int length();
    int GetBufferBlockCount();

    // Save to file
    RESULT SaveToFile(char *pszFilename, bool fOverwreite);

private:
    char *m_pBuffer;
    int m_pBuffer_n;
    int m_pBuffer_bn;
};
    
} // namespace dss

#endif // _SMARTBUFFER_H