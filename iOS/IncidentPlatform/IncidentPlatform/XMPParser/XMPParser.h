#pragma once

// The XMP parser will take the output of the XMPTokenizer and 
// convert it into a list of discernable XMP elements
#include "XMPTokenizer.h"
#include "RESULT.h"
#include "EHM.h"
#include "XMPElement.h"

class XMPParser :
    public valid
{
public:
    XMPParser() :
      m_pTokenizer(NULL)
      {
          //RESULT r = R_SUCCESS;

          // Create the element list
          m_pElements = new list<XMPElement*>();

          Validate();
          return;
Error:
          Invalidate();
          return;
      }

    XMPParser(const char* pszFile) :
      m_pTokenizer(NULL)
    {
        RESULT r = R_SUCCESS;
        
        // Tokenize the file outputs a list of tokens
		// The parser then converts this into a list of
		// elements
        m_pTokenizer = new XMPTokenizer(pszFile);

        CVRM(m_pTokenizer, "XMPParser: Tokenizer for file %s was invalid", pszFile);
        
        // Create the element list
        m_pElements = new list<XMPElement*>();
		ParseTokens();

        //for(list<XMPElement*>::iterator it = m_pElements->First(); it != NULL; it++)
        //{
            //reinterpret_cast<XMPElement*>(*it)->PrintElement();
        //}

        Validate();
        return;
    Error:
        Invalidate();
        return;
    }

	XMPTokenizer *GetXMPTokenizer(){ return m_pTokenizer; }
    
    // Will return the index of the character in the string before
    // hitting the end of the string or a specific delimiter provided
    // by delim.  Instance is a variable that will allow finding the #instance
    // occurance of said character
    // Using instance <= 0 will find the last instance of the character in a block 
    static int FindCharInString(const char *pszString, char c, char delim, int instance);

    // Will remove whitespace at beginning and end of string
    char *RemoveWhitespacePads(char* &dr_pszSrc);
    static char* RemoveWhitespace(char *pszSrc);

    RESULT ExtractAttributes(char* pszToken, list<XMPElement*> *plistElements);
	
    
    RESULT ParseTokens();
    RESULT ParseTokens(char *pszString);

    list<XMPElement*>*GetElementList(){ return m_pElements; }

private:
    list<XMPElement*> *m_pElements;
    XMPTokenizer *m_pTokenizer;
};