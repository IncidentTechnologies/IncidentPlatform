#pragma once

// FlatObject is the interface for any object in the iEngine that is
// flat and is in reference of the visible window.  This is effectively
// the 2D engine component which can be used for in game graphics or 
// a 2D side scroller or GUI based application.  This object does not
// handle the functional component but only the actual graphical 
// component.  

typedef enum FlatObjectType
{
    FLAT_OBJECT,
    FLAT_BLIT,
    FLAT_TRI,
    FLAT_CIRCLE,
    FLAT_LINE,
    FLAT_TEXT,
    FLAT_COMMON_CONTROL,
    FLAT_INVALID    // place holder (end of enum list)
} FLAT_OBJECT_TYPE;

#include "point.h"
#include "vertex.h"
//#include "texture.h"
#include "material.h"     // once the material manager is impleneted it will be possible to assign complex materials and effects
#include "DimensionImp.h"
#include "dirty.h"

class blit;

typedef unsigned long FLAT_ID;

static FLAT_ID g_FlatObjCount = 0;

class FlatObject :
    public material,
    public dirty
{
public:
    FlatObject(FLAT_OBJECT_TYPE fot);
    FlatObject(FLAT_OBJECT_TYPE fot, int ScreenWidth, int ScreenHeight);
    FlatObject(point ptOrigin, FLAT_OBJECT_TYPE fot);
    FlatObject(point ptOrigin, FLAT_OBJECT_TYPE fot, int ScreenWidth, int ScreenHeight);
    FlatObject(const FlatObject &fo);
    ~FlatObject(){/* empty stub */}

    void RotateX(float fx);
    void RotateY(float fy);
    void RotateZ(float fz);
    void SetRotateX(float fx);
    void SetRotateY(float fy);
    void SetRotateZ(float fz);
    void SetPosition(point pt);
    void SetX(float x);
    void SetY(float y);
    void SetZ(float z);
    void MoveX(float x);
    void MoveY(float y);
    void MoveZ(float z);

    // Convert the origin, dimensions, and properties to the screen
    // This MUST be implemented
    virtual RESULT ConvertToScreen() = 0;
    virtual const char *ToString() = 0;

    // This function's purpose is to return a blit that contains the Flat Object completely
    // much like an axis aligned bounding box for the Dim Objects
    virtual blit GetBlit() = 0;

    // This is used by both the generic implementations and the API specific
    // implementation to initialize the required internals of the object
    virtual RESULT InitializeFlatObject(void *pContext) = 0;

    // These are needed for abstraction later
    // none of the dimension objects need to implement this
    // but the implementation objects will overload it
    virtual RESULT Initialize(DimensionImp *pDimImp) = 0;
    virtual RESULT Render(DimensionImp *pDimImp) = 0;

    // These functions can be overloaded but is not required
    virtual RESULT ConvertToScreen(int ScreenWidth, int ScreenHeight);
    virtual RESULT SetAttribute(int Attribute, void *pContext);
    virtual RESULT GetAttribute(int Attribute, void * &r_pContext);

    int GetScreenWidth(){ return m_ScreenWidth; }
    int GetScreenHeight(){ return m_ScreenHeight; }
    void SetAxisAligned(bool AxisAligned);
    void Scale(float fScale){ m_Scale = fScale; }
    FLAT_OBJECT_TYPE GetObjectType(){ return m_FlatObjectType; }
    FLAT_ID GetID(){ return m_id; }
    point GetOrigin(){ return m_ptOrigin; }


    virtual vertex* GetVertexBuffer() = 0;
    virtual int GetVertexCount() = 0;
    RESULT SetVertexColor(color c)
    {
        RESULT r = R_SUCCESS;

        vertex *VertexBuffer = GetVertexBuffer();
        int VertexBuffer_n = GetVertexCount();
        
        if((VertexBuffer != NULL && VertexBuffer_n == 0) ||
           (VertexBuffer == 0    && VertexBuffer_n > 0 ))
        {
            CBRM(0, "FlatObject: SetColor failed due to invalid vertex buffer or count");
        }

        if(VertexBuffer != NULL && VertexBuffer_n > 0)
            for(int i = 0; i < VertexBuffer_n; i++)        
                VertexBuffer[i].SetColor(c);        

Error:
        return r;
    }

public:
    point m_ptOrigin;
    float m_RotationX;
    float m_RotationY;
    float m_RotationZ;
    float m_Scale;
    bool m_AxisAligned_f;

protected:
    bool m_Screen_f;
    int m_ScreenWidth;
    int m_ScreenHeight;

private:
    FLAT_ID m_id;
    FLAT_ID IncrementFlatObjCount()
    {
        g_FlatObjCount++;
        return g_FlatObjCount;
    }

protected:
    FLAT_OBJECT_TYPE m_FlatObjectType;
};



