// IDimObject is the interface for any object in Dimension that contains
// any kind of dimensional information of more than simply a few points
// or lines.
// Example:
// Dimensional Objects:  Triangle, Sphere, Volume ... etc
// Non-Dimensional Objects:  Point, Vector, Ray, Line ... etc

#pragma once

typedef enum dimension_object_type
{
    DIM_OBJECT,
    DIM_CAMERA,
    DIM_LIGHT,
    DIM_TRIANGLE,
    DIM_PLANE,
    DIM_SPHERE,
    DIM_VOLUME,
	DIM_TEXT,
    DIM_RECTANGLE,
    DIM_TEXT3D,
    DIM_MESH_MD3,
    DIM_MESH_OBJ,
    DIM_HTERRAIN,
    DIM_INVALID
} DIM_OBJECT_TYPE;

#include "point.h"
#include "vertex.h"
//#include "texture.h"
#include "material.h"
#include "dirty.h"

// Dimension ID set as a unsigned long and maintained here
// this allows for a potential of 2^32 (~4 GigaObject) objects
typedef unsigned long DIM_ID;

// Dimension Object Counter
static DIM_ID g_DimObjCount = 0;

class plane;

class DimObject :
    public dirty
    //public texture
    //public material
{   
public:
    DimObject(DIM_OBJECT_TYPE dot, material *pMaterial = NULL, bool fMaterial = true) :
        //material(),                      // initialize textures
        m_DimObjType(dot),
        m_ptOrigin(),
        m_fRotationX(0.0f),
        m_fRotationY(0.0f),
        m_fRotationZ(0.0f),
        m_fScale(1.0f),
        m_AxisAligned_f(false),
        m_pMaterial(NULL)
    {  
        m_id = IncrementDimObjCount();

        if(pMaterial != NULL)
            m_pMaterial = pMaterial;
        else if(fMaterial)
            m_pMaterial = new material();
    }

    DimObject(point ptOrigin, DIM_OBJECT_TYPE dot, material *pMaterial = NULL, bool fMaterial = true) :
        //material(),                      // initialize textures
        m_DimObjType(dot),  
        m_ptOrigin(ptOrigin),
        m_fRotationX(0.0f),
        m_fRotationY(0.0f),
        m_fRotationZ(0.0f),
        m_fScale(1.0f),
        m_AxisAligned_f(false),
        m_pMaterial(NULL)
    {  
        m_id = IncrementDimObjCount();

        if(pMaterial != NULL)
            m_pMaterial = pMaterial;
        else if(fMaterial)
            m_pMaterial = new material();
    }

    ~DimObject()
    {
        if(m_pMaterial != NULL)
        {
            delete m_pMaterial;
            m_pMaterial = NULL;
        }
    }

    void RotateX(float fX) {
        if(!m_AxisAligned_f) m_fRotationX += fX; 
    }
    
    void SetRotateX(float fX) {
        if(!m_AxisAligned_f) m_fRotationX = fX; 
    }
    
    void RotateY(float fY) {
        if(!m_AxisAligned_f) m_fRotationY += fY; 
    }
    
    void SetRotateY(float fY) {
        if(!m_AxisAligned_f) m_fRotationY = fY; 
    }
    
    void RotateZ(float fZ) {
        if(!m_AxisAligned_f) m_fRotationZ += fZ; 
    }
    
    void SetRotateZ(float fZ) {
        if(!m_AxisAligned_f) m_fRotationZ = fZ; 
    }

    void SetX(float x){ m_ptOrigin.tuple[DIM_X] = x; }
    void SetY(float y){ m_ptOrigin.tuple[DIM_Y] = y; }
    void SetZ(float z){ m_ptOrigin.tuple[DIM_Z] = z; }

    void MoveX(float x){ m_ptOrigin.tuple[DIM_X] += x; }
    void MoveY(float y){ m_ptOrigin.tuple[DIM_Y] += y; }
    void MoveZ(float z){ m_ptOrigin.tuple[DIM_Z] += z; }

    void SetAxisAligned(bool AxisAligned)
    {
        if(AxisAligned)
        {
            m_fRotationX = 0.0f;
            m_fRotationY = 0.0f;
            m_fRotationZ = 0.0f;            
        }
        m_AxisAligned_f = AxisAligned;
    }

    void Scale(float fScale){ m_fScale = fScale; }

    DIM_OBJECT_TYPE GetObjectType(){ return m_DimObjType; }

    DIM_ID GetID(){ return m_id; }

    virtual const char *ToString() = 0;
    
    virtual plane *GetPlane()
    {
        return NULL;
    }

    // These are needed for abstraction later
    // none of the dimension objects need to implement this
    // but the implementation objects will overload it
    //virtual RESULT Initialize(DimensionImp *pDimImp) = 0;
    //virtual RESULT Render(DimensionImp *pDimImp) = 0;

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
            CBRM(0, "DimObject: SetColor failed due to invalid vertex buffer or count");
        }

        for(int i = 0; i < VertexBuffer_n; i++)        
            VertexBuffer[i].SetColor(c);        

Error:
        return r;
    }

    // Moving the Material out of inheritance and into an object but keeping the interface
    // this way the Material can be implementation specific but the interface stays the same
    texture GetTexture(TEXTURE_TYPE tt)
    { 
        if(m_pMaterial != NULL)
            return m_pMaterial->GetTexture(tt); 
        else
        {
            printf("Tried to access uninitialized material\n");
            return texture();
        }
    }

    RESULT AddTexture(char *pszFilename, TEXTURE_TYPE tt)
    { 
        if(m_pMaterial != NULL)
            return m_pMaterial->AddTexture(pszFilename, tt);
        else
        {
            printf("Tried to access uninitialized material\n");
            return R_FAIL;
        }
    }

    int GetTextureCount(TEXTURE_TYPE tt)
    {
        if(m_pMaterial != NULL)
            return m_pMaterial->GetTextureCount(tt);
        else
        {
            printf("Tried to access uninitialized material\n");
            return -1;
        }
    }

    RESULT SetMaterialParameter(MATERIAL_PARAM mp, void *pValue)
    {
        if(m_pMaterial != NULL)
            return m_pMaterial->SetMaterialParameter(mp, pValue);
        else
        {
            printf("Tried to access uninitialized material\n");
            return R_FAIL;
        }
    }

    SHADE_STYLE GetMaterialShadeStyle()
    {
        return m_pMaterial->m_ShadeStyle;
    }

    RESULT GetMaterialParameter(MATERIAL_PARAM mp, void* &r_pValue)
    {
        if(m_pMaterial != NULL)
            return m_pMaterial->GetMaterialParameter(mp, r_pValue);
        else
        {
            printf("Tried to access uninitialized material\n");
            return R_FAIL;
        }
    }


public:
    point m_ptOrigin;

    material *m_pMaterial;      // This is implemented by the constructor 

    float m_fRotationX;
    float m_fRotationY;
    float m_fRotationZ;

    float m_fScale;

    bool m_AxisAligned_f;

private:
    DIM_ID m_id;

    DIM_ID IncrementDimObjCount()
    {
        g_DimObjCount++;
        return g_DimObjCount;        
    }

// Textures are now inherited by ALL DimObjects
// textures and such
//public:
   // texture m_texture;

protected:
    DIM_OBJECT_TYPE m_DimObjType;
};