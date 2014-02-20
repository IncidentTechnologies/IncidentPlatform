#include "XMPTest.h"
#include "SmartBuffer.h"
#include <stdio.h>
#include <string>
#include <stdlib.h>

XMPTree *g_pXMPTree = NULL;

RESULT InitXMPTests(Console *pc)
{
    RESULT r = R_SUCCESS;

    
    
    pc->AddConsoleFunction((void*)(TestSampleInput), (char*)("XMPSampleInput"), 2, 0);
    pc->AddConsoleFunction((void*)(CreateTree), (char*)"CreateXMPTree", 1, 0);
    pc->AddConsoleFunction((void*)PrintTree, (char*)"PrintXMPTree", 1, 0);
    pc->AddConsoleFunction((void*)AddNode, (char*)"AddXMPNode", 2, 0);
    pc->AddConsoleFunction((void*)NavigateToChildByName, (char*)"NavigateToXMPChildByName", 2, 0);
    pc->AddConsoleFunction((void*)NavigateToParent, (char*)"NavigateToXMPParent", 1, 0);
    pc->AddConsoleFunction((void*)AppendContent, (char*)"AppendXMPContent", 2, 0);
    pc->AddConsoleFunction((void*)AddAttributeNameValue, (char*)"AddXMPAttribute", 3, 0);
    pc->AddConsoleFunction((void*)SaveXMPToFile, (char*)"SaveXMPToFile", 2, 0);
    pc->AddConsoleFunction((void*)AppendDataToXMP, (char*)"AppendDataToXMP", 3, 0);
	pc->AddConsoleFunction((void*)OpenXMPFile, (char*)"OpenXMPFile", 2, 0);

Error:
    return r;
}

RESULT OpenXMPFile(Console *pc, char *pszFilename)
{
	RESULT r = R_SUCCESS;
	
	g_pXMPTree = new XMPTree(pszFilename);
	CNRM(g_pXMPTree, "Failed to create XMP Tree!");

Error:
	return r;
}

RESULT AppendDataToXMP(Console *pc, char *pszDataName, char *pszDataID)
{
    RESULT r = R_SUCCESS;
    long int DataID = 0;
    
    CNRM(g_pXMPTree, "Error: XMP  tree is null!");
    DataID = atoi(pszDataID);

    // Create the test structure
    XMPHandlerTestStruct xhts;
    xhts.id = DataID;
    memset(xhts.szString, 0, sizeof(xhts.szString));
    strcpy(xhts.szString, "XMP Test Struct");
    for(int i = 0; i < (sizeof(xhts.rgInt) / sizeof(int)); i++)
        xhts.rgInt[i] = i;
    xhts.flSpecial = 23.53f;

    g_pXMPTree->AppendData(pszDataName, DataID, (void*)(&xhts), sizeof(xhts));

Error:
    return r;
}

RESULT SaveXMPToFile(Console *pc, char *pszFilename)
{
    RESULT r = R_SUCCESS;

    CNRM(g_pXMPTree, "Error: XMP  tree is null!");
    CRM(g_pXMPTree->SaveXMPToFile(pszFilename, true), "Error: XMP tree failed to save to file");

Error:
    return r;
}

RESULT TestSampleInput(Console *pc, char *pszFilename)
{
    RESULT r = R_SUCCESS;

    pc->PrintToOutput("Trying to open %s\r\n", pszFilename);
    g_pXMPTree = new XMPTree(pszFilename);

Error:
    return r;
}

RESULT PrintTree(Console *pc)
{
    RESULT r = R_SUCCESS;

    SmartBuffer *pTemp = NULL;
    CNRM(g_pXMPTree, "Error: XMP  tree is null!");
    CRM(g_pXMPTree->PrintXMPTree(pTemp), "Failed to print out XMP Tree");
    pc->PrintToOutput(pTemp->GetBuffer());
    
Error:
    if(pTemp != NULL)
        delete pTemp;

    return r;
}

RESULT CreateTree(Console *pc)
{
    RESULT r = R_SUCCESS;

    g_pXMPTree = new XMPTree();
    CNRM(g_pXMPTree, "Failed to create XMP Tree!");

Error:
    return r;
}

// Add node adds a node at the current location in the tree
RESULT AddNode(Console *pc, char *pszNodeName)
{
    RESULT r = R_SUCCESS;
    CNRM(g_pXMPTree, "Error: XMP  tree is null!");
    CRM(g_pXMPTree->AddChildByName(pszNodeName), "AddNode: Node failed to add into tree");
Error:
    return r;
}

RESULT NavigateToChildByName(Console *pc, char *pszNodeName)
{
    RESULT r = R_SUCCESS;
    CNRM(g_pXMPTree, "Error: XMP  tree is null!");
    CRM(g_pXMPTree->NavigateToChildName(pszNodeName), "NavigateToChildByName: Could not find child %s", pszNodeName);
Error:
    return r;
}

RESULT NavigateToParent(Console *pc)
{
    RESULT r = R_SUCCESS;
    CNRM(g_pXMPTree, "Error: XMP  tree is null!");
    CRM(g_pXMPTree->NavigateToParent(), "NavigateToParent: No parent available");
Error:  
    return r;
}

RESULT AppendContent(Console *pc, char *pszContent)
{
    RESULT r = R_SUCCESS;
    CNRM(g_pXMPTree, "Error: XMP  tree is null!");
    CRM(g_pXMPTree->AppendContent(pszContent), "AppendContent: Failed to append content");
Error:
    return r;
}

RESULT AddAttributeNameValue(Console *pc, char *pszName, char *pszValue)
{
    RESULT r = R_SUCCESS;
    CNRM(g_pXMPTree, "Error: XMP  tree is null!");
    CRM(g_pXMPTree->AddAttributeByNameValue(pszName, pszValue), "AddAttributeNameValue: Failed to add attribute by name and value");
Error:
    return r;
}