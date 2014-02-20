#pragma once

#include "MeshObj.h"
#include "GLES2DimObject.h"
#include "EHM.h"

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

using namespace dss;

class GLES2MeshObj :
    public MeshObj,
    public GLES2DimObject
{
public:
    GLES2MeshObj() :
        MeshObj()
    {/*empty stub*/}

    GLES2MeshObj(char *pszFilepath) :
        MeshObj(pszFilepath)
    {/*empty stub*/}
    
    GLES2MeshObj(DimObject *obj) :
        MeshObj(static_cast<MeshObj*>(obj)->GetFilepath())
    {/*empty stub*/}
    
    ~GLES2MeshObj() {/*empty stub*/}
    
public:
    RESULT GLES2Init() {
        RESULT r = R_SUCCESS;
        
        glGenBuffers(1, &m_VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, m_verts_n * sizeof(vertex), m_verts, GL_STATIC_DRAW);
        
        glGenBuffers(1, &m_IndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicies_n * sizeof(unsigned short), m_indicies, GL_STATIC_DRAW);
        
        return r;
    }
    
    RESULT GLES2Update() {
        RESULT r = R_SUCCESS;
        void* pVoid;
        
        /*D3DXMATRIX matTemp;
        
        D3DXMatrixIdentity(&m_matTransform);
        
        if( m_fScale != 1.0f )
        {
            // Scale only if scale is not 1
            //D3DMatrixScale(&m_matTransform);
        }
        
        if( m_fRotationX != 0.0f )
        {
            D3DXMatrixRotationX(&matTemp, m_fRotationX);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }
        if(m_fRotationY != 0.0f)
        {
            D3DXMatrixRotationY(&matTemp, m_fRotationY);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }
        if(m_fRotationZ != 0.0f )
        {
            D3DXMatrixRotationZ(&matTemp, m_fRotationZ);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }
        if( m_ptOrigin != point(0.0f, 0.0f, 0.0f))
        {
            // Translate only if not at origin anyways
            D3DXMatrixTranslation(&matTemp, m_ptOrigin.tuple[0], m_ptOrigin.tuple[1], m_ptOrigin.tuple[2]);
            D3DXMatrixMultiply(&m_matTransform, &m_matTransform, &matTemp);
        }
        
        // lock the buffer and load in the vertices's
        m_pd39Buf->Lock(0, 0, (void**)&pVoid, 0);
        memcpy(pVoid, m_tuple, sizeof(m_tuple));
        m_pd39Buf->Unlock();
        */
         
        return r;
    }
    
    
    RESULT Render(GLuint position, GLuint color) {
        // Set Vertex Format
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
        glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) (sizeof(float) * 3 * 3));
        
        // Send Geometry
        glDrawElements(GL_TRIANGLES, m_indicies_n, GL_UNSIGNED_SHORT, 0);
        
        return R_SUCCESS;
    }
    
public:
    GLuint m_VertexBuffer;
    GLuint m_IndexBuffer;
    
};
