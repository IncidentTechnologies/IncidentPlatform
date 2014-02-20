#include "XMPTokenizer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "XMPTest.h"
#include "XMPParser.h"  // We do micro parse the XMP to look for data pockets

XMPTokenizer::XMPTokenizer(const char *pszFile) :
    m_pszFile(NULL),
    m_pFile(NULL)
{
    m_pszFile = new char[strlen(pszFile)];
    strcpy(m_pszFile, pszFile);
    
    if(m_pszFile == NULL)
    {
        printf("Input file name is NULL\n");
        goto Error;
    }

    m_pFile = fopen(m_pszFile, "r+b");

    if(m_pFile == NULL) 
    {
        printf("Could not open file:%s\n", pszFile);
        goto Error;
    }

    m_pTokenList = new list<XMPToken*>;

    TokenizeFile(XMP_TOKENIZER_DELIMS);

    Validate();
    return;
Error:
    Invalidate();
}

XMPTokenizer::~XMPTokenizer()
{
    fclose(m_pFile);
}

#define TOKEN_BLOCK_SIZE 256

XMPDataToken *filetok(FILE *pFile, long int size, int dataid, char *pszName)
{
    RESULT r = R_SUCCESS;

    XMPDataToken *pTempToken = new XMPDataToken();
    pTempToken->xtt = XMP_TOKEN_DATA;
    void *pBuffer = (void*)malloc(size);
    pTempToken->pBuffer = pBuffer;
    pTempToken->dataid = dataid;
    pTempToken->pszDataName = pszName;
    pTempToken->datasize = size;
    int bytesRead = 0;

    char cHS = fgetc(pFile);  
    CBRM((cHS == 42), "XMPTokenizer:filtok handshake character was not magic!");


    bytesRead = fread(pBuffer, 1, size, pFile);
    if(bytesRead != size) {
        CBRM(0, "Failed to read %d bytes, only read %d bytes", size, bytesRead);
    }
    else
    {
        // Reset handshake char and Jog forward if needed
        cHS = fgetc(pFile);     
        CBRM((cHS == 42), "XMPTokenizer:filtok end handshake character was not magic!");
        return pTempToken;
    }

Error:    
    return NULL;
}

char* filetok(FILE *pFile, const char* delims)
{
    SmartBuffer Token;

    char InChar = fgetc(pFile);

    if(InChar == EOF)
    {
        // If first character checked is EOF immediately return NULL
        return NULL;
    }

    while(InChar != EOF)
    {
        char TempChar = InChar;
        bool Delimiter_f = false; 
        
        for(int i = 0; i < strlen(delims); i++)
        {
            if(InChar == delims[i])
            {
                Delimiter_f = true;
                if(Token.GetBufferLength() > 0)              
                    return Token.CreateBufferCopy();                
                else                
                    break;                
            }
        }

        if(!Delimiter_f)
            Token += TempChar;

        InChar = fgetc(pFile);
    }

    if(Token.GetBufferLength() > 0)
        return Token.CreateBufferCopy();
    else
        return NULL;
}

RESULT XMPTokenizer::TokenizeFile( const char* XMPTokenizerDelims )
{
    RESULT r = R_SUCCESS;

    bool fInsideDataTag = false;
    char *TokenizerTemp = NULL;
    long int datasize = -1;
    int dataid = -1;
    char *pszName = NULL;

    CPRM(m_pFile, "File is null!\n");

    TokenizerTemp = filetok(m_pFile, XMPTokenizerDelims);

    while(TokenizerTemp != NULL)
    {     
        //printf("token:%s\n", TokenizerTemp);

        XMPParser *pTempParser = new XMPParser();
        pTempParser->ParseTokens(TokenizerTemp);

        XMPTextToken *pTextToken = new XMPTextToken();
        pTextToken->xtt = XMP_TOKEN_TEXT;
        pTextToken->pszContent = TokenizerTemp;
        m_pTokenList->Append(pTextToken);
        
        // Make sure there are elements
        if(pTempParser->GetElementList()->length() > 0) {

            // Check here for data node
            XMPElement *pTempElement = *(pTempParser->GetElementList()->First());
            if(pTempElement->GetElementType() == XMP_START_TAG && strcmp(XMPParser::RemoveWhitespace(pTempElement->GetName()), "data") == 0) {
                // This is a data node (get some attributes!)
                for(list<XMPElement*>::iterator it = pTempParser->GetElementList()->First();
                    it != NULL;
                    it++)
                {
                    if(strcmp("dataname", XMPParser::RemoveWhitespace((*it)->GetName())) == 0  )             
                        pszName = (*it)->GetValue();                
                    else if(strcmp("dataid", XMPParser::RemoveWhitespace((*it)->GetName())) == 0 )               
                        dataid = atoi((*it)->GetValue());                
                    else if(strcmp("datasize", XMPParser::RemoveWhitespace((*it)->GetName())) == 0)                
                        datasize = atol((*it)->GetValue());                                
                }

                // Now read the data node out of the XMP here
                XMPDataToken *pDataToken = filetok(m_pFile, datasize, dataid, pszName);
                m_pTokenList->Append(pDataToken);
            }
        }

        TokenizerTemp = filetok(m_pFile, XMPTokenizerDelims);

        delete pTempParser;
        pTempParser = NULL;
    }

Error:
    return r;
}