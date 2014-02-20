#pragma once

#include "dss_list.h"
#include "XMPTree.h"

#include "Console.h"

extern XMPTree *g_pXMPTree;

#define XMP_HANDLER_TEST_STRUCT_ID 14253

typedef struct XMPHandlerTestStruct
{
    int id;
    char szString[50];
    int rgInt[50];
    float flSpecial;
};

// Initializes all of the XMP stuffs
RESULT InitXMPTests(Console *pc);

// XMP Test and Manipulation Functions
RESULT TestSampleInput(Console *pc, char *pszFilename);
RESULT CreateTree(Console *pc);
RESULT PrintTree(Console *pc);
RESULT AddNode(Console *pc, char *pszNodeName);
RESULT NavigateToChildByName(Console *pc, char *pszNodeName);
RESULT NavigateToParent(Console *pc);
RESULT AddAttributeNameValue(Console *pc, char *pszName, char *pszValue);
RESULT AppendContent(Console *pc, char *pszContent);

RESULT SaveXMPToFile(Console *pc, char *pszFilename);

RESULT AppendDataToXMP(Console *pc, char *pszDataName, char *pszDataID);

RESULT OpenXMPFile(Console *pc, char *pszFilename);