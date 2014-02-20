#include "cmnPriv.h"
#include <string>

using namespace dss;

RESULT CreateRect(UINT iLeft, UINT iTop, UINT iRight, UINT iBottom, RECT *rc) {
    RESULT r = R_SUCCESS;

    CBR((rc != NULL));

    rc->left = (long)iLeft;
    rc->right = (long)iRight;
    rc->top = (long)iTop;
    rc->bottom = (long)iBottom;

Error:
    return r;
}

RESULT SetConsoleHWND(const TCHAR* pszTitle) {
    RESULT r = R_SUCCESS;

#ifdef __WIN32__
    SetConsoleTitle(pszTitle);
    Sleep(5);
#else
    r = R_UNSUPPORTED;
#endif

Error:
    return r;
    
}

HWND GetConsoleHWND(const TCHAR* pszTitle) {
    HWND hwnd;
#ifdef __WIN32__
    hwnd = FindWindow(NULL, pszTitle);
    return hwnd;
#else
    return NULL;
#endif
}

// This function will expand a static buffer with an expansion buffer and will handle the
// memory allocation issues.  This is similar to a memcat except this combines buffers and will NOT
// delete the expansion buffer.  This deletes the original static buffer but will allocate memory for it
// so the existing memory location will no longer be valid.
// Decision was made to stop deleting the expansion buffer since this may be needed somewhere else
RESULT ExpandStaticBuffer(void * &dm_StaticBuffer, int &dm_StaticBuffer_n, void *Expansion, int Expansion_n) {
    RESULT r = R_SUCCESS;

    void *TempBuffer = (void*)malloc(dm_StaticBuffer_n + Expansion_n);
    void *TempBuffer_cur = (char*)TempBuffer + dm_StaticBuffer_n;

    memcpy(TempBuffer, dm_StaticBuffer, dm_StaticBuffer_n);
    memcpy(TempBuffer_cur, Expansion, Expansion_n );

    delete dm_StaticBuffer;
    //delete d_Expansion;

    dm_StaticBuffer = TempBuffer;
    dm_StaticBuffer_n += Expansion_n;

Error:
    return r;
}

RESULT CheckFileExistance(char *pszFilename) {
    RESULT r = R_SUCCESS;
    FILE *file = fopen(pszFilename, "r");

    if(file == NULL) {
        r = R_FAIL;
    }
    else {
        fclose(file);
        r = R_SUCCESS;
    }

Error:
    return r;
}