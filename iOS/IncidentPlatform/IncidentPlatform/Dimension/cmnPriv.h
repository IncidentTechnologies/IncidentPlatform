#pragma once

// Choose Graphics API
//#define DIMUSE_DIRECTX9
//#define DIMUSE_OPENGL

#include "KeyTable.h"
#include "RESULT.h"

// Default window size:
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

#define PI 3.141592653589793

#include <stdio.h>

#include "EHM.h"
#include "DSS.h"

// Dimension and DirectXDimension
//#include "DimensionPrimitives.h"
//#include "DX9Primitives.h"
//#include "GLWnd.h"
//#include "DXWnd.h"

// Some windows defines
#ifndef UINT
    typedef unsigned int UINT;
#endif

#ifndef LONG
    typedef long int LONG;
#endif

#ifndef HANDLE
    typedef void* HANDLE;
#endif

#ifndef HWND
    typedef HANDLE HWND;
#endif

#ifndef TCHAR
    typedef char TCHAR;
#endif

#ifndef RECT
    typedef struct _RECT {
        LONG left;
        LONG top;
        LONG right;
        LONG bottom;
    } RECT, *PRECT;
#endif

using namespace dss;

RESULT CreateRect(UINT iLeft, UINT iTop, UINT iRight, UINT iBottom, RECT *rc);

RESULT SetConsoleHWND(const TCHAR* pszTitle);
HWND GetConsoleHWND(const TCHAR* pszTitle);

RESULT ExpandStaticBuffer(void * &dm_StaticBuffer, int &dm_StaticBuffer_n, void *Expansion, int Expansion_n);

// Size struct and type (for floats / strings etc)
// this is not to be used for objects!
typedef enum IncidentType {
    INCIDENT_BYTE,
    INCIDENT_CHAR,
    INCIDENT_SHORT,
    INCIDENT_INT    
} INCIDENT_TYPE;

RESULT CheckFileExistance(char *pszFilename);