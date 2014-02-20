#pragma once

#include "dss_list.h"
#include "SmartBuffer.h"
#include "valid.h"

// XMP Tokenizer will tokenize the XML header portion of the XMP file
// and save it as an internal list of tokens.  The delimiters should be
// global but the tokenizer will also provide for adjusting these parameters

// Attributes must always be quoted (" or ')
// and have a = right before them so they are easy to predict
// " \r\t\r" - whitespace tokens
// <> Tag indicators, seperate tokens
// = seperates attributes from attribute values (in quotes)
#define XMP_TOKENIZER_DELIMS "\n\t\r"

using namespace dss;
class XMPParser;    // to define this here so we can use it later

typedef enum XMPTokenType
{
    XMP_TOKEN_TEXT,
    XMP_TOKEN_DATA,
    XMP_TOKEN_INVALID
} XMP_TOKEN_TYPE;

typedef struct XMPToken
{
    XMP_TOKEN_TYPE xtt;
} XMP_TOKEN;

typedef struct XMPDataToken : public XMPToken
{
    char *pszDataName;
    void *pBuffer;
    int dataid;
    long int datasize;
} XMP_DATA_TOKEN;

typedef struct XMPTextToken : public XMPToken {
    char *pszContent;
} XMP_TEXT_TOKEN;

class XMPTokenizer :
    public valid
{
public:
    XMPTokenizer(const char *pszFile);
    ~XMPTokenizer();

    list<XMPToken*>* GetTokenList(){ return m_pTokenList; }

private:
    RESULT TokenizeFile(const char* XMPTokenizerDelims);
    
private:
    char *m_pszFile;
    FILE *m_pFile;
    
    list<XMPToken*> *m_pTokenList;

};