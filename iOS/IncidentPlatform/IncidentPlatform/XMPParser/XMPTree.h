#pragma once

// XMP Tree takes an XMP Parser object as an input / or creates
// one internally.  Then it will create the appropriate XMP
// Tree that can then be navigated 

#include "XMPNode.h"
#include "XMPParser.h"
#include "dss_list.h"


class XMPTree : public valid {
public:
    // XMP can be created out of an existing file
    XMPTree(char* pszFilename);
    XMPTree();      // XMP can be created out of nothing
    ~XMPTree();

    RESULT ConstructTree();
    
    RESULT PrintXMPDepth(int depth, SmartBuffer* &n_psmbuf, FILE *pFile);
    RESULT PrintXMPChar(char c, SmartBuffer* &n_psmbuf, FILE *pFile);
    RESULT PrintXMPStr(char *psz, SmartBuffer* &n_psmbuf, FILE *pFile);
    RESULT PrintXMPTree(SmartBuffer* &n_psmbuf);
    RESULT PrintXMPTree();
    RESULT PrintXMPTree(XMPNode *node, int depth, SmartBuffer* &n_psmbuf, FILE *pFile = NULL);
    
    RESULT SaveXMPToFile(char *pszFilename, bool fOverwrite);

    // Navigation Functions
    RESULT ResetNavigator();
    RESULT NavigateToChildName(char *pszName);
    RESULT NavigateToParent();
    RESULT AddChildByName(char *pszName);
    RESULT AddChild(XMPNode* node);
    
    RESULT AddAttributeByNameValue(char *pszName, char *pszValue);
    RESULT AddAttribute(XMPAttribute *attrib);
    RESULT AppendContent(char *pszContent);
    
    // Navigation Functions
    XMPNode *NavNode();
    
    list<XMPNode*>*GetChildrenAtNav();
    list<XMPAttribute*>*GetAttributesAtNav();

    // AppendData is a more complex hybrid action which will append a new node and fill it
    // with the data buffer passed
    RESULT AppendData(char *pszDataName, long int DataID, void *pBuffer, long int pBuffer_s);

    XMPNode *GetRootNode();

private:
    // Parser Object for use to parse out the file
    XMPParser *m_pParser;
    
    char *m_pszFilename;

    // Root node of the tree
    XMPNode *m_pRoot;

    // Tree Cursor for Navigation
    XMPNode *m_pNodeNav;
};

