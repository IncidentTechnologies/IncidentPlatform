#include "Text3D.h"
#include <string>
#include <stdlib.h>

using namespace dss;

Font3D* Text3D::InitFont3D() {
    struct Font3D *ret = new Font3D();
    
    ret->name = new char[strlen(DEFAULT_FONT3D_NAME)];
    strcpy(ret->name, DEFAULT_FONT3D_NAME);
    
    ret->height = DEFAULT_FONT3D_HEIGHT;
    ret->width = DEFAULT_FONT3D_WIDTH;
    ret->depth = DEFAULT_FONT3D_DEPTH;
    
    return ret;
}

RESULT Text3D::Initialize(const char* c_str)
{
    /* TODO WHAT?! */
    m_pBuffer = (char *)malloc(DEFAULT_TEXT3D_BUFFER_SIZE);
    memset(m_pBuffer, 0, DEFAULT_TEXT3D_BUFFER_SIZE);
    if(c_str != NULL) {
        strcpy(m_pBuffer, c_str);
    }
    
    m_pFont = InitFont3D();
    
    return R_SUCCEED;
}