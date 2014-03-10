#include "XMPParser.h"
#include <string>

RESULT XMPParser::ExtractAttributes( char* pszToken, list<XMPElement*> *plistElements)
{
    RESULT r = R_SUCCESS;

    //printf("Token:%s\n", pszToken);

    SmartBuffer *pTokenBuffer = new SmartBuffer();
    XMP_ELEMENT_TYPE xetTemp = XMP_INVALID;

    for(int i = 0; i < strlen(pszToken); i++)
    {
        char CurChar = pszToken[i];
        bool Delimit_f = false;

        switch(CurChar)
        {
        case ' ':
            {
                if(xetTemp == XMP_START_TAG && pTokenBuffer->length() > 0)
                {
                    // Hit a white space after a beginning tag
                    // this should mean that this represents a start-tag
                    XMPElement *pTempStartTag = new XMPElement(XMP_START_TAG, pTokenBuffer->CreateBufferCopy());
                    //pTempStartTag->PrintElement();
                    plistElements->Append(pTempStartTag);
                    xetTemp = XMP_INVALID;
                    pTokenBuffer->Reset();
                }
                else 
                {
                    // Otherwise we just add it to the smart buffer
                    *pTokenBuffer += CurChar;
                }
            } break;

        case '<':
            {
                if(pTokenBuffer->length() > 0 && pTokenBuffer->NotAllWhitespace())
                {
                    // Then there is likely data
                    char *TempContentValue = pTokenBuffer->CreateBufferCopy();
                    TempContentValue = RemoveWhitespacePads(TempContentValue);
                    XMPElement *pTempContentElement = new XMPElement(XMP_CONTENT, NULL, TempContentValue);
                    //pTempContentElement->PrintElement();
                    plistElements->Append(pTempContentElement);
                    xetTemp = XMP_INVALID;
                    pTokenBuffer->reset();
                }
                xetTemp = XMP_START_TAG;
            } break;

        case '>':
            {
                // If we encounter a > before a / and a start tag has not been identified this must be 
                // an attribute and as such requires parsing as an attribute
                if(xetTemp == XMP_INVALID && pTokenBuffer->length() > 0)
                {                                            
                    char *TempToken = pTokenBuffer->CreateBufferCopy();
                    char *TempTokenName = NULL;
                    char *TempTokenValue = NULL;
                    int AttributeCount = 1;
                    int Equals = FindCharInString(TempToken, '=', '>', AttributeCount);

                    int FirstQuotes = 0;
                    int SecondQuotes = 0;
                    int SecondQuotesAbs = 0;

                    while(Equals != -1)
                    {                                                    
                        TempTokenName = NULL;
                        TempTokenValue = NULL;

                        // Name
                        //int Equals = FindCharInString(TempToken, '=', '>', 1);
                        TempTokenName = new char[Equals - SecondQuotesAbs + 1]; 
                        strncpy(TempTokenName, TempToken + SecondQuotesAbs, (Equals - SecondQuotesAbs));
                        TempTokenName[Equals - SecondQuotesAbs] = '\0';
                        TempTokenName = RemoveWhitespacePads(TempTokenName);

                        // Value
                        FirstQuotes = FindCharInString(TempToken + Equals, '"', '>', 1);
                        SecondQuotes = FindCharInString(TempToken + Equals, '"', '>', 2);
                        SecondQuotesAbs = SecondQuotes + Equals + 1;    // add one as to not copy over the quotes
                        TempTokenValue = new char[SecondQuotes - FirstQuotes];
                        strncpy(TempTokenValue, TempToken + Equals + FirstQuotes + 1, SecondQuotes - FirstQuotes);
                        TempTokenValue[SecondQuotes - FirstQuotes - 1] = '\0';
                        TempTokenValue = RemoveWhitespacePads(TempTokenValue);

                        //printf("TempTokenName:%s TempTokenValue:%s\n", TempTokenName, TempTokenValue);
                        XMPElement *pTempTagAttribute = new XMPElement(XMP_TAG_ATTRIBUTE, TempTokenName, TempTokenValue);
                        //pTempTagAttribute->PrintElement();
                        plistElements->Append(pTempTagAttribute);
                        xetTemp = XMP_INVALID;
                        pTokenBuffer->reset();

                        AttributeCount++;                            
                        Equals = FindCharInString(TempToken, '=', '>', AttributeCount);
                    }
                }
                else if(xetTemp == XMP_START_TAG)
                {
                    // Have completed a start tag
                    XMPElement *pTempStartTag = new XMPElement(XMP_START_TAG, pTokenBuffer->CreateBufferCopy());
                    //pTempStartTag->PrintElement();
                    plistElements->Append(pTempStartTag);
                    xetTemp = XMP_INVALID;
                    pTokenBuffer->reset();
                }
                else if(xetTemp == XMP_END_TAG)
                {
                    // Completed an end tag
                    XMPElement *pTempEndTag = new XMPElement(XMP_END_TAG, pTokenBuffer->CreateBufferCopy());
                    //pTempEndTag->PrintElement();
                    plistElements->Append(pTempEndTag);
                    xetTemp = XMP_INVALID;
                    pTokenBuffer->reset();
                }

            } break;

        case '/':
            {
                if(pszToken[i + 1] == '>')
                {                                                
                    // We need to check if this is an attribute element and if it is 
                    // we need to manipulate the most recently added tag to be an empty tag
                    // and set this to an attribute tag
                    if(FindCharInString(pTokenBuffer->GetBuffer(), '=', '>', 1) != -1)
                    {
                        //printf("ATTRIBUTE!\n");

                        // This means that a previous start tag was incorrectly 
                        // created and should be converted to an empty tag
                        for(list<XMPElement*>::iterator it = plistElements->Last(); it != NULL; it--)
                        {
                            if(dynamic_cast<XMPElement*>(*it)->GetElementType() == XMP_START_TAG)
                            {
                                dynamic_cast<XMPElement*>(*it)->SetElementType(XMP_EMPTY);
                                break;
                            }
                        }

                        xetTemp = XMP_INVALID;
                        break;
                    }

                    XMPElement *pTempEmptyTag = new XMPElement(XMP_EMPTY, pTokenBuffer->CreateBufferCopy());
                    //pTempEmptyTag->PrintElement();
                    plistElements->Append(pTempEmptyTag);
                    xetTemp = XMP_INVALID;
                    pTokenBuffer->reset();
                }
                else if(xetTemp == XMP_INVALID)
                {
                    if(pszToken[i + 1] == '>')  // Doubt this will be an issue, but ensures that we only close a tag when / is followed by >
                        xetTemp = XMP_END_TAG;
                }                    
                else if(xetTemp == XMP_START_TAG)
                {
                    xetTemp = XMP_END_TAG;
                }
                else
                {
                    printf("Warn '\\' char found where it doesn't belong! \n");
                }
            } break;   

        default: {
                *pTokenBuffer += CurChar;
            } break;
        }
    }

    // If there is something in the buffer then it must be some kind of content
    if(xetTemp == XMP_INVALID && pTokenBuffer->length() > 0 && pTokenBuffer->NotAllWhitespace())
    {
        char *TempContentValue = pTokenBuffer->CreateBufferCopy();
        TempContentValue = RemoveWhitespacePads(TempContentValue);
        XMPElement *pElement = new XMPElement(XMP_CONTENT, NULL, TempContentValue);
        //pElement->PrintElement();
        plistElements->Append(pElement);
    }        

Error:
    delete pTokenBuffer;
    //delete pElement;
    return r;
}

char* XMPParser::RemoveWhitespace( char *pszSrc )
{
    int FirstChar = 0;
    int LastChar = 0;
    char *pszReturn = NULL;

    for(FirstChar = 0; FirstChar < strlen(pszSrc); FirstChar++)        
        if(pszSrc[FirstChar] != ' ' && pszSrc[FirstChar] != '\t')            
            break;                    

    if((LastChar = FindCharInString(pszSrc + FirstChar, ' ', '\0', 1)) == -1)        
        LastChar = strlen(pszSrc);

    pszReturn = new char[LastChar + 1];
    memset(pszReturn, 0, LastChar + 1);
    strncpy(pszReturn, pszSrc + FirstChar, LastChar);       

Error:       
    return pszReturn;
}

char * XMPParser::RemoveWhitespacePads( char* &dr_pszSrc ) {
    int FirstChar = 0;
    int LastChar = 0;
    char *pszReturn = NULL;

    for(FirstChar = 0; FirstChar < strlen(dr_pszSrc); FirstChar++)
    {
        if(dr_pszSrc[FirstChar] != ' ' && dr_pszSrc[FirstChar] != '\t')            
            break;            
    }

    if((LastChar = FindCharInString(dr_pszSrc + FirstChar, ' ', '\0', 0)) == -1)
    {
        LastChar = strlen(dr_pszSrc);
    }

    pszReturn = new char[LastChar + 1];
    memset(pszReturn, 0, LastChar + 1);
    strncpy(pszReturn, dr_pszSrc + FirstChar, LastChar);       

Error:
    delete [] dr_pszSrc;        
    return pszReturn;
}

int XMPParser::FindCharInString( const char *pszString, char c, char delim, int instance )
{
    int length = strlen(pszString);
    for(int ret = 0; ret < length; ret++)
    {
        if(pszString[ret] == c)
        {
            instance--;
            if(instance < 0)
            {
                for(int i = ret; i < length; i++)
                {
                    if(pszString[i] != c)
                    {
                        instance = 0;
                        ret = i;
                        break;
                    }
                }     

                if(instance < 0)
                    return ret;
            }
            else if(instance == 0)                
                return ret;                
        }
        else if(pszString[ret] == delim)
        {
            printf("FindCharInString::Error!\n");
            return -1;
        }                        
    }
    return -1;
}

RESULT XMPParser::ParseTokens(char *pszString)
{
    RESULT r = R_SUCCESS;

    CRM(ExtractAttributes(pszString, m_pElements), "XMPParser:ParseTokens failed to extract attributes");

Error:
    return r;
}

RESULT XMPParser::ParseTokens()
{
    RESULT r = R_SUCCESS;

    for(list<XMPToken*>::iterator it = m_pTokenizer->GetTokenList()->First();
        it != NULL;
        it++)
    {
        if((*it)->xtt == XMP_TOKEN_TEXT)
        {
            char *temp = static_cast<XMPTextToken*>(*it)->pszContent;
            CRM(ExtractAttributes(temp, m_pElements), "XMPParser:ParseTokens failed to extract attributes");
        }      
        else if((*it)->xtt == XMP_TOKEN_DATA)
        {
            XMPDataToken *pTempToken = (XMPDataToken*)(*it);
            XMPElement *pTemp = new XMPElement(pTempToken->pBuffer, pTempToken->datasize, pTempToken->dataid, pTempToken->pszDataName);
            CRM(m_pElements->Append(pTemp), "XMPParser: Failed to append data element");
        }
    }

Error:
    return r;
}