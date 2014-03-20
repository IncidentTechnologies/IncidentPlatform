//
//  CircularBuffer.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/19/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef __IncidentPlatform__CircularBuffer__
#define __IncidentPlatform__CircularBuffer__

namespace dss {

template <class T>
class CircularBuffer {
public:
    
    CircularBuffer(float msLength, float SamplingFreq) :
        m_msLength(0),
        m_pBuffer(NULL),
        m_pBuffer_n(0)
    {
        SetLength(msLength, SamplingFreq);
    }
    
    CircularBuffer(unsigned long bufferLength) :
        m_msLength(0),
        m_pBuffer(NULL),
        m_pBuffer_n(0)
    {
        SetLength(bufferLength);
    }
    
    ~CircularBuffer() {
        if(m_pBuffer != NULL) {
            delete [] m_pBuffer;
            m_pBuffer = NULL;
        }
    }
    
    RESULT SetLength(unsigned long length) {
        RESULT r = R_SUCCEED;
        
        if(m_pBuffer != NULL) {
            delete [] m_pBuffer;
            m_pBuffer = NULL;
            m_pBuffer_n = 0;
        }
        
        m_pBuffer_n = length;
        m_pBuffer = new T[length];
        m_pBuffer_c = 0;
        m_pBuffer_e = m_pBuffer_n - 1;
        
        for(unsigned long i = 0; i < m_pBuffer_n; i++)
            m_pBuffer[i] = 0;
        
        CNRM(m_pBuffer, "CircularBuffer: SetLength failed to create new buffer");
        
    Error:
        return r;
    }
    
    T PushElement(T element) {
        T outElem = m_pBuffer[m_pBuffer_c];
        m_pBuffer[m_pBuffer_c] = element;
        
        m_pBuffer_c += 1;
        if(m_pBuffer_c >= m_pBuffer_n)
            m_pBuffer_c = 0;
        
        m_pBuffer_e += 1;
        if(m_pBuffer_e >= m_pBuffer_n)
            m_pBuffer_e = 0;
        
        return outElem;
    }
    
    RESULT SetLength(float msLength, float SamplingFreq) {
        unsigned long length = (unsigned long)((float)((msLength / 1000.0f)) *  (float)(SamplingFreq));
        return SetLength(length);
    }
    
    T& operator[](const unsigned long& i) {
        return m_pBuffer[i];
    }
    
    const T& operator[](const unsigned long& i) const {
        if(i < m_pBuffer_n)
            return m_pBuffer[i];
        else
            return NULL;
    }
    
    T GetBufferAtPosition(unsigned long pos) {
        if(pos < m_pBuffer_n)
            return m_pBuffer[pos];
        else
            return NULL;
    }
    
    unsigned long length(){
        return m_pBuffer_n;
    }
    
private:
    float m_msLength;
    
    T* m_pBuffer;
    
    unsigned long m_pBuffer_n;  // length
    unsigned long m_pBuffer_c;  // current
    unsigned long m_pBuffer_e;  // end
};

} // namespace dss

#endif /* defined(__IncidentPlatform__CircularBuffer__) */
