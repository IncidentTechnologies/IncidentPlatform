	#include "XMPTree.h"
#include <string>
#include <stdlib.h>





/* XMPTree **************/
XMPTree::XMPTree(char* pszFilename) :
  m_pszFilename(pszFilename)
{
    RESULT r = R_SUCCESS;
    
    m_pRoot = new XMPNode("root", NULL);
    m_pNodeNav = m_pRoot;
    
    if(m_pszFilename != NULL) {
        m_pParser = new XMPParser(m_pszFilename);    
        CVRM(m_pParser, "XMPTree: XMPParser is invalid for %s file", pszFilename);
        ConstructTree();
        delete m_pParser;
    }

    Validate();
    return;
Error:
    Invalidate();
    return;
}

// XMP can be created out of nothing
XMPTree::XMPTree() :
  m_pszFilename(NULL)
{
    m_pRoot = new XMPNode("root", NULL);
    m_pNodeNav = m_pRoot;
}

XMPTree::~XMPTree() {
    // TODO: Implement this!
}

// Constructs tree based on the parser component
RESULT XMPTree::ConstructTree() {
    RESULT r = R_SUCCESS;

    list<XMPElement*>*Elements = m_pParser->GetElementList();        
    list<XMPElement*>::iterator it = Elements->First();
    XMPNode *CurrentNode = m_pRoot;
    XMPNode *CurrentEmptyNode = NULL;

    while(it != NULL) {
        //(*it)->PrintElement();  // debug: print out the list for fun

      switch((*it)->GetElementType()) {
        case XMP_START_TAG: {
            // In this case we crate a new tree node and move to that level
            if(CurrentEmptyNode != NULL)
                CurrentEmptyNode = NULL;

            XMPNode *TempNode = new XMPNode((*it)->GetName(), CurrentNode);
            CNRM(TempNode, "XMPTree.ConstructTree: Failed to create a new start tag node");
            CurrentNode->AddChild(TempNode);
            CurrentNode = TempNode;                    
        } break;

        case XMP_END_TAG: {
            // In this case we jump out into the previous level
            CurrentNode = CurrentNode->GetParent();
            CNRM(CurrentNode, "XMPTree.ConstructTree: End tag failure, current node is null!");
        } break;

        case XMP_EMPTY: {
            // In this case we create a new tree node but don't jump into it
            // need to set a pointer so if any attributes belong to the empty
            // tag they are correctly set.  This pointer is set to NULL anytime
            // another tag is encountered
            CurrentEmptyNode = NULL;        // reset just in case
            CurrentEmptyNode = new XMPNode((*it)->GetName(), CurrentNode);
            CNRM(CurrentEmptyNode, "XMPTree.ConstructTree: Failed to create a new empty tag node");
            CurrentNode->AddChild(CurrentEmptyNode);
        } break;

        case XMP_CONTENT: {
            // Creates a new content node and appends it to the current node (no need to enter the level)
            if(CurrentEmptyNode != NULL)
                CurrentEmptyNode = NULL;
            XMPNode *TempNode = new XMPNode("content", CurrentNode, (*it)->GetValue());
            CNRM(TempNode, "XMPTree.ConstructTree: Failed to create a new content node");
            CurrentNode->AddChild(TempNode);
        } break;

        case XMP_CONTENT_DATA: {
            // If an XMP Content Data node is encountered we will append just the data node to the current node
            // which should be a start tage
            //XMPNode *DataNode = new XMPNode()
            CNRM((*it)->GetBuffer(), "Data node has NULL buffer!");
            CBRM(((*it)->GetBufferSize() != 0), "Data node has zero sized data!");

            XMPNode *pData = new XMPNode("data", NULL, NULL, (*it)->GetBuffer(), (*it)->GetBufferSize());
            CRM(CurrentNode->AddChild(pData), "AppendData: Failed to append Data Content");
        } break;

        case XMP_TAG_ATTRIBUTE: {
            XMPAttribute *TempAttribute = new XMPAttribute((*it));
            CNRM(TempAttribute, "XMPTree.ConstructTree: Failed to create a new attribute");
            // Attributes can be given to empty nodes so we check if we have one
            if(CurrentEmptyNode != NULL)                    
                CurrentEmptyNode->AddAttribute(TempAttribute);                    
            else                    
                CurrentNode->AddAttribute(TempAttribute);                    
        } break;                

        case XMP_INVALID: {
            printf("XMPTree.ConstructTree: Invalid element type!\n");
        } break;

        default: {
            printf("XMPTree.ConstructTree: Could not add element type!\n");
        } break;

      };

      it++;     // Increment our point in the list of nodes
    }

    if(CurrentNode != m_pRoot) {
        CBRM(0, "XMP Error! No end tag found for %s", CurrentNode->GetName());
    }

Error:
    return r;
}

RESULT XMPTree::PrintXMPDepth(int depth, SmartBuffer* &n_psmbuf, FILE *pFile) {
    if(depth != 0)
        for(int i = 0; i < depth; i++)                     
            PrintXMPStr("    ", n_psmbuf, pFile);        

    return R_SUCCESS;
}

RESULT XMPTree::PrintXMPChar(char c, SmartBuffer* &n_psmbuf, FILE *pFile) {
    if(n_psmbuf != NULL) 
        *n_psmbuf += c;
    else if(pFile != NULL)
        fputc(c, pFile);
    else
        printf("%c", c);

    return R_SUCCESS;
}

RESULT XMPTree::PrintXMPStr(char *psz, SmartBuffer* &n_psmbuf, FILE *pFile) {
    if(n_psmbuf != NULL)
        *n_psmbuf += psz;
    else if(pFile != NULL)
        fputs(psz, pFile);
    else
        printf("%s", psz);

    return R_SUCCESS;
}

// Also output a smart buffer with the contents of the 
RESULT XMPTree::PrintXMPTree( XMPNode *node, int depth, SmartBuffer* &n_psmbuf, FILE *pFile) {
    RESULT r = R_SUCCESS;    

    if(node->IsContentNode()) {
        PrintXMPDepth(depth, n_psmbuf, pFile);
        PrintXMPStr(node->GetContentCopy(), n_psmbuf, pFile);
        PrintXMPChar('\n', n_psmbuf, pFile);
    }
    else if(node->IsDataNode()) {
        if(pFile == NULL)
            PrintXMPDepth(depth, n_psmbuf, pFile);

        char *tempChRg = new char[3];  

        if(pFile != NULL) {
            fputc((char)42, pFile);
            //fputs("data", pFile);
            int byteswritten = fwrite(node->m_pDataBuffer, 1, node->m_DataBuffer_s, pFile);
            CBRM((byteswritten == node->m_DataBuffer_s), "Written bytes %d and size %d do not match", byteswritten, node->m_DataBuffer_s);
            //fputs("enddata", pFile);
            fputc((char)42, pFile);
        }
        else {
            for(int i = 0; i < node->m_DataBuffer_s; i++) {                                           
                sprintf(tempChRg, "%0X\0", (unsigned long int)(static_cast<unsigned long int*>(node->m_pDataBuffer) + i));
                PrintXMPStr(tempChRg, n_psmbuf, pFile);                

                // Pop in a new line every 22 numbers
                if((i + 1) % 30 == 0) {                
                    PrintXMPChar('\n', n_psmbuf, pFile);
                    PrintXMPDepth(depth, n_psmbuf, pFile);
                }                
            }
        }

        PrintXMPChar('\n', n_psmbuf, pFile);

        delete [] tempChRg;
        tempChRg = NULL;

    }
    else {        
        PrintXMPDepth(depth, n_psmbuf, pFile);
        PrintXMPChar('<', n_psmbuf, pFile);
        PrintXMPStr(node->GetName(), n_psmbuf, pFile);

        //if(node == m_pNodeNav)
        //    PrintXMPChar('*', n_psmbuf, pFile);


        for(list<XMPAttribute*>::iterator attribit = node->GetAttributes()->First(); attribit != NULL; attribit++) {                        
            PrintXMPChar(' ', n_psmbuf, pFile);
            PrintXMPStr((*attribit)->GetName(), n_psmbuf, pFile);
            PrintXMPStr("=\"", n_psmbuf, pFile);
            PrintXMPStr((*attribit)->GetXMPValue().GetPszValue(), n_psmbuf, pFile);
            PrintXMPChar('\"', n_psmbuf, pFile);           
        }

        if(node->GetChildren()->Size() != 0) {            
            PrintXMPStr(">\n", n_psmbuf, pFile);

            for(list<XMPNode*>::iterator kidit = node->GetChildren()->First(); kidit != NULL; kidit++)
                PrintXMPTree((*kidit), depth + 1, n_psmbuf, pFile);

            PrintXMPDepth(depth, n_psmbuf, pFile);
            PrintXMPStr("</", n_psmbuf, pFile);
            PrintXMPStr(node->GetName(), n_psmbuf, pFile);
            PrintXMPStr(">\n", n_psmbuf, pFile);           
        }
        else {
            // must be an empty tag            
            PrintXMPStr(" />\n", n_psmbuf, pFile);
        }
    }

Error:
    return r;
}

RESULT XMPTree::PrintXMPTree(SmartBuffer* &n_psmbuf) {         
    n_psmbuf = new SmartBuffer();
    return PrintXMPTree(*m_pRoot->GetChildren()->First(), 0, n_psmbuf);
}

RESULT XMPTree::PrintXMPTree() {         
    SmartBuffer *pNulBuf = NULL;
    return PrintXMPTree(*m_pRoot->GetChildren()->First(), 0, pNulBuf);
}

RESULT XMPTree::SaveXMPToFile(char *pszFilename, bool fOverwrite) {
    RESULT r = R_SUCCESS;
    
    FILE *pFile = fopen(pszFilename, "r");
    if(pFile == NULL || fOverwrite) {
        if(pFile != NULL)
            fclose(pFile);
        pFile = fopen(pszFilename, "wb");
        
        SmartBuffer *pTemp = NULL;
        CRM(PrintXMPTree(*m_pRoot->GetChildren()->First(), 0, pTemp, pFile), "SaveXMPToFile: Faild due to PrintXMPTree Failure");            
    }
    else        
        CBRM(0, "File %s already exists or overwrite flag not set!", pszFilename);        
    
    //CRM(pNulBuf->SaveToFile(pszFilename, fOverwrite), "SaveXMPToFile: Failed due to SaveToFile failure");

Error:
    if(pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }
    return r;
}

// Navigation Functions
RESULT XMPTree::ResetNavigator() {
    m_pNodeNav = m_pRoot;
    return R_SUCCESS;
}

RESULT XMPTree::NavigateToChildName(char *pszName) {
    XMPNode *temp = m_pNodeNav->FindChildByName(pszName);
    if(temp != NULL) {
        m_pNodeNav = temp;
        return R_XMP_NODE_FOUND;
    }
    else
        return R_XMP_NODE_NOT_FOUND;
}

RESULT XMPTree::NavigateToParent() {
    XMPNode *temp = m_pNodeNav->GetParent();
    if(temp != NULL) {
        m_pNodeNav = temp;
        return R_XMP_NODE_FOUND;
    }
    else
        return R_XMP_NO_PARENT;
}

RESULT XMPTree::AddChildByName(char *pszName) {
    RESULT r = R_SUCCESS;

    XMPNode *pTemp = new XMPNode(pszName, m_pNodeNav, NULL);
    CRM(m_pNodeNav->AddChild(pTemp), "XMPTree:AddChildByName Failed to add child");
Error:
    return r;
}

RESULT XMPTree::AddAttributeByNameValue(char *pszName, char *pszValue) {
    RESULT r = R_SUCCESS;

    XMPAttribute *pTempAtrib = new XMPAttribute(pszName, pszValue);
    CNRM(pTempAtrib, "AddAttributeByNameValue: Failed to allocate attribute");
    CRM(AddAttribute(pTempAtrib), "AddAttributeByName: Failed to add attribute of name %s", pszName);
Error:
    return r;
}

RESULT XMPTree::AddAttribute(XMPAttribute *attrib) {
    RESULT r = R_SUCCESS;
    
    CRM(m_pNodeNav->AddAttribute(attrib), "XMPTree:AddAttribute Failed to add attribute");

Error:
    return r;
}

RESULT XMPTree::AppendContent(char *pszContent) {
    RESULT r = R_SUCCESS;

    XMPNode *pTemp = new XMPNode("content", NULL, pszContent);
    CRM(m_pNodeNav->AddChild(pTemp), "XMPTree:AppendContent Failed to append content");

Error:
    return r;
}

// AppendData is a more complex hybrid action which will append a new node and fill it
// with the data buffer passed
RESULT XMPTree::AppendData(char *pszDataName, long int DataID, void *pBuffer, long int pBuffer_s) {
    RESULT r = R_SUCCESS;
    
    XMPNode *pDataNode = NULL;
    XMPAttribute *pDataNameAttrib = NULL;
    XMPAttribute *pDataIDAtrib = NULL;
    XMPAttribute *pSizeAttrib = NULL;
    void *pTempBuffer = NULL;
    XMPNode *pData = NULL;

    pDataNode = new XMPNode("data", m_pNodeNav, NULL);
    CRM(m_pNodeNav->AddChild(pDataNode), "XMPTree:AppendData Failed to add data node");

    // Add the name attribute
    pDataNameAttrib = new XMPAttribute("dataname", pszDataName);
    CNRM(pDataNameAttrib, "AppendData: Failed to allocate the data name attribute");
    CRM(pDataNode->AddAttribute(pDataNameAttrib), "AppendData: Failed to add the data name attribute");

    // Add the DataID attribute 
    pDataIDAtrib = new XMPAttribute("dataid", DataID);
    CNRM(pDataIDAtrib, "AppendData: Failed to allocate data id attribute");
    CRM(pDataNode->AddAttribute(pDataIDAtrib), "AppendData: Failed to DataID attribute"); 

    // Add the Data Size attribute
    pSizeAttrib = new XMPAttribute("datasize", pBuffer_s);
    CNRM(pSizeAttrib, "AppendData: Failed to allocate data size attribute");
    CRM(pDataNode->AddAttribute(pSizeAttrib), "AppendData: Failed to add data size attribute");

    //Create a new char buffer out of the information
    // We need to make a copy of this data and give it to the node in case the memory is
    // deallocated somewhere else
    pTempBuffer = (void*)malloc(pBuffer_s);
    CNRM(pTempBuffer, "XMPTree:AppendData: failed to allocate node buffer size %d", pBuffer_s);
    memcpy(pTempBuffer, pBuffer, pBuffer_s);

    pData = new XMPNode("data", NULL, NULL, pTempBuffer, pBuffer_s);
    CRM(pDataNode->AddChild(pData), "AppendData: Failed to append Data Content");

Error:
    return r;
}

XMPNode* XMPTree::NavNode() {
    return m_pNodeNav;
}

XMPNode* XMPTree::GetRootNode(){
  return m_pRoot; 
}

list<XMPNode*>* XMPTree::GetChildrenAtNav() {
    return m_pNodeNav->GetChildren();
}

list<XMPAttribute*>* XMPTree::GetAttributesAtNav() {
    return m_pNodeNav->GetAttributes();
}




