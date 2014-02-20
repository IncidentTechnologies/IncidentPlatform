#include "SmartBuffer.h"
#include <string>

using namespace dss;

SmartBuffer::SmartBuffer( const char *pBuffer, int pBuffer_n) :
    m_pBuffer(NULL),
    m_pBuffer_n(0),
    m_pBuffer_bn(0)
{
    if(pBuffer == NULL)
        ResetBuffer();
    else if(pBuffer_n == 0)
        ResetBuffer(pBuffer);
    else
        ResetBuffer(pBuffer, pBuffer_n);
}

SmartBuffer::SmartBuffer(SmartBuffer *psb) :
    m_pBuffer(NULL),
    m_pBuffer_n(0),
    m_pBuffer_bn(0)
{
    ResetBuffer(psb->GetBuffer(), psb->GetBufferLength());
}

SmartBuffer::~SmartBuffer() {
    if(m_pBuffer != NULL) {
        delete [] m_pBuffer;            
        m_pBuffer = NULL;
    }
    m_pBuffer_n = 0;
    m_pBuffer_bn = 0;
}

RESULT SmartBuffer::IncrementBufferSize() {
    RESULT r = R_SUCCESS;
    char *SaveBuffer = NULL;

    CPRM(m_pBuffer, "SmartBuffer is not initialized!\n");
    CBRM((m_pBuffer_n > 0), "SmartBuffer size is 0!\n");

    SaveBuffer = new char[m_pBuffer_n];
    CNRM(SaveBuffer, "SmartBuffer: Failed to allocate memory for save pBuffer");
    memcpy(SaveBuffer, m_pBuffer, m_pBuffer_n);

    delete [] m_pBuffer;
    m_pBuffer = NULL;
    m_pBuffer_bn++;
    m_pBuffer = new char[SMART_BUFFER_BLOCK_SIZE * (m_pBuffer_bn + 1)];
    CNRM(m_pBuffer, "SmartBuffer: Failed to allocate memory for new pBuffer");
    memset(m_pBuffer, 0, SMART_BUFFER_BLOCK_SIZE * (m_pBuffer_bn + 1));

    memcpy(m_pBuffer, SaveBuffer, m_pBuffer_n);

    delete [] SaveBuffer;
    SaveBuffer = NULL;

Error:
    return r;
}

RESULT SmartBuffer::ResetBuffer( const char *pszBuffer ) {
    return ResetBuffer(pszBuffer, strlen(pszBuffer));
}

RESULT SmartBuffer::ResetBuffer( const char *pBuffer, int pBuffer_n ) {
    RESULT r = R_SUCCESS;

    CRM(ResetBuffer(), "ResetBuffer: Failed to reset pBuffer");

    for(int i = 0; i < pBuffer_n; i++)
        CRM(Append(pBuffer[i]), "ResetBuffer: Failed to append %c", pBuffer[i]);

Error:
    return r;
}

RESULT SmartBuffer::ResetBuffer() {
    RESULT r = R_SUCCESS;

    m_pBuffer_n = 0;
    m_pBuffer_bn = 0;

    if(m_pBuffer != NULL)
        delete [] m_pBuffer;

    m_pBuffer = new char[(m_pBuffer_bn + 1)*SMART_BUFFER_BLOCK_SIZE];
    memset(m_pBuffer, 0, (m_pBuffer_bn + 1)*SMART_BUFFER_BLOCK_SIZE);

Error:
    return r;
}

RESULT SmartBuffer::Reset() {
    return ResetBuffer();
}

RESULT SmartBuffer::reset() {
    return ResetBuffer();
}

char * SmartBuffer::CreateBufferCopy() {
    char *temp = new char[m_pBuffer_n + 1];
    memset(temp, 0, m_pBuffer_n + 1);
    memcpy(temp, m_pBuffer, m_pBuffer_n);

    return temp;
}

bool SmartBuffer::NotAllWhitespace() {
    for(int i = 0; i < m_pBuffer_n; i++) {
        if(m_pBuffer[i] != ' ' && 
            m_pBuffer[i] != '\t') {
            return true;
        }
    }
    return false;
}

RESULT SmartBuffer::Append( char byte ) {
    RESULT r = R_SUCCESS;

    if(m_pBuffer == NULL) {
        m_pBuffer_bn = 0;
        m_pBuffer_n = 0;
        m_pBuffer = new char[(m_pBuffer_bn + 1)*SMART_BUFFER_BLOCK_SIZE];
        memset(m_pBuffer, 0, (m_pBuffer_bn + 1)*SMART_BUFFER_BLOCK_SIZE);
    }

    *(m_pBuffer + m_pBuffer_n) = byte;
    m_pBuffer_n++;

    if(m_pBuffer_n >= (m_pBuffer_bn + 1) * SMART_BUFFER_BLOCK_SIZE)
        CRM(IncrementBufferSize(), "SmartBuffer: Failed to increment the pBuffer size!");

Error:
    return r;
}

RESULT SmartBuffer::Append( SmartBuffer sb ) {
    RESULT r = R_SUCCESS;
    
    char *TempBuffer = sb.GetBuffer();
    for(int i = 0; i < sb.GetBufferLength(); i++)
        
        CRM(Append(TempBuffer[i]), "SmartBuffer: Failed to append: %c", TempBuffer[i]);
Error:
    return r;
}

char* SmartBuffer::GetBuffer() {
    return m_pBuffer;
}

int SmartBuffer::GetBufferLength() {
    return m_pBuffer_n;
}

int SmartBuffer::Length() {
    return m_pBuffer_n;
}

int SmartBuffer::length() {
    return m_pBuffer_n;
}

int SmartBuffer::GetBufferBlockCount() {
    return m_pBuffer_bn;
}

SmartBuffer & SmartBuffer::operator=( const SmartBuffer &rhs ) {
    if(this != &rhs) {
        delete [] this->m_pBuffer;
        this->m_pBuffer_n = rhs.m_pBuffer_n;
        this->m_pBuffer_bn = rhs.m_pBuffer_bn;
        this->m_pBuffer = new char[(this->m_pBuffer_bn + 1) * SMART_BUFFER_BLOCK_SIZE];
        memset(this->m_pBuffer, 0, (this->m_pBuffer_bn + 1) * SMART_BUFFER_BLOCK_SIZE);

        memcpy(this->m_pBuffer, rhs.m_pBuffer, this->m_pBuffer_n);
    }

    return *this;
}

SmartBuffer & SmartBuffer::operator+=( char byte ) {
    this->Append(byte);
    return *this;
}

SmartBuffer & SmartBuffer::operator+=( const SmartBuffer &rhs ) {
    this->Append(rhs);
    return *this;
}

SmartBuffer& SmartBuffer::operator+=(const char *psz) {
    for(int i = 0; psz[i] != 0; i++)
        this->Append(psz[i]);
    return *this;
}

const SmartBuffer SmartBuffer::operator+( const char byte ) {
    SmartBuffer *result = new SmartBuffer(this->m_pBuffer, this->m_pBuffer_n);
    result->Append(byte);
    return *result;
}

const SmartBuffer SmartBuffer::operator+(const char *psz) {
    SmartBuffer *result = new SmartBuffer(this->m_pBuffer, this->m_pBuffer_n);
    
    for(int i = 0; psz[i] != 0; i++)    
        result->Append(psz[i]);
    
    return *result;
}

SmartBuffer & SmartBuffer::operator--(int) {
    char *TempLastChar = this->m_pBuffer + m_pBuffer_n - 1;
    this->RemoveCharacter(TempLastChar);
 
    return *this;
}

const SmartBuffer SmartBuffer::operator-( const int n ) {
    SmartBuffer *result = new SmartBuffer(this);
 
    for(int i = 0; i < n; i++)
        (*result)--;
    
    return *result;
}


SmartBuffer & SmartBuffer::operator-=( int n ) {
    for(int i = 0; i < n; i++)
        (*this)--;
    return *this;
}

const SmartBuffer SmartBuffer::operator-( const char byte ) {
    SmartBuffer *result = new SmartBuffer(this);
    char *pTempChar;
    
    if(result->Find(byte, pTempChar) == R_SUCCESS)
        result->RemoveCharacter(pTempChar);
    return result;
}

const SmartBuffer SmartBuffer::operator %( const char byte ) {
    SmartBuffer *result = new SmartBuffer(this);
    char *pTempChar;
    while(result->Find(byte, pTempChar) == R_SUCCESS)
        result->RemoveCharacter(pTempChar);
    return result;
}

SmartBuffer &SmartBuffer::operator %=(const char byte) {
    char *pTempChar;
    while(this->Find(byte, pTempChar) == R_SUCCESS)
        this->RemoveCharacter(pTempChar);
    return *this;
}

SmartBuffer &SmartBuffer::operator-=(const char byte) {
    char *pTempChar;
    if(this->Find(byte, pTempChar) == R_SUCCESS)
        this->RemoveCharacter(pTempChar);
    return *this;
}

const SmartBuffer SmartBuffer::operator+( const SmartBuffer &rhs ) {
    SmartBuffer *result = new SmartBuffer(this->m_pBuffer, this->m_pBuffer_n);
    result->Append(rhs);
    return *result;
}

RESULT SmartBuffer::RemoveCharacter( char *pChar ) {
    RESULT r = R_SUCCESS;

    if((pChar < m_pBuffer) || (pChar > m_pBuffer + ((m_pBuffer_bn + 1) * SMART_BUFFER_BLOCK_SIZE)))
        CBRM(0, "RemoveCharacter: Char pointer out of the bounds of the SmartBuffer Buffer");

    // Two cases, either char is last character (then it's easy!) or not
    if(pChar == m_pBuffer + m_pBuffer_n) {
        m_pBuffer[m_pBuffer_n] = '\0';
        m_pBuffer_n--;
    }
    else {
        char *TempBuffer = new char[strlen(pChar + 1)];
        strcpy(TempBuffer, pChar + 1);
        memset(pChar, 0, m_pBuffer_n - (pChar - m_pBuffer));
        strcat(m_pBuffer, TempBuffer);
        m_pBuffer_n--;
    }    

Error:
    return r;
}


// Finds the first instance of byte and returns a pointer to it
RESULT SmartBuffer::Find( const char byte, char *&r_pChar) {
    RESULT r = R_SUCCESS;

    r_pChar = m_pBuffer;

    while((*r_pChar) != '\0' && (*r_pChar) != byte)
        r_pChar++;

    if((*r_pChar) == byte)
        r = R_SUCCESS;
    else
        r = R_SB_CHAR_NOT_FOUND;

Error:
    return r;
}

RESULT SmartBuffer::SaveToFile(char *pszFilename, bool fOverwrite = false) {
    RESULT r = R_SUCCESS;

    // First check to see file does not exist already
    FILE *pFile = fopen(pszFilename, "r");
    if(pFile == NULL || fOverwrite) {
        if(pFile != NULL)
            fclose(pFile);

        pFile = fopen(pszFilename, "w");
        fputs(m_pBuffer, pFile);
        fclose(pFile);
    }
    else {
        fclose(pFile);
        CBRM(0, "File %s already exists!", pszFilename);
    }

Error:
    return r;
}