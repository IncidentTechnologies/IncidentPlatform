#pragma once

#include "triangle.h"
#include "GLES2DimObject.h"
#include "EHM.h"

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "MatrixFactory.h"

using namespace dss;

class GLES2Triangle :
    public triangle,
    public GLES2DimObject
{
public:
    GLES2Triangle() :
        triangle()
    {/*empty stub*/}
    
    GLES2Triangle(TRIANGLE_STYLE iTriStyle) :
        triangle(iTriStyle)
    {/*empty stub*/}
    
    GLES2Triangle(vertex *tuple) :
        triangle(tuple)
    {/*empty stub*/}
    
    GLES2Triangle(DimObject *obj) :
        triangle(static_cast<triangle*>(obj)->GetTuple())
    {/*empty stub*/}
    
    ~GLES2Triangle()
    {
        //m_pd39Buf->Release();
    }
    
public:
    RESULT GLES2Init() {
        RESULT r = R_SUCCESS;
        
        // Set up the VBO for the triangle
        glGenBuffers(1, &m_VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_tuple), m_tuple, GL_STATIC_DRAW);
        
        glGenBuffers(1, &m_IndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indicies), m_indicies, GL_STATIC_DRAW);
        
        return r;
    }
    
    RESULT GLES2Update(GLuint modelView) {
        RESULT r = R_SUCCESS;
        
        matrix<float> modelViewMat = CreateIdentity(4);
        
        /*
        if( m_fScale != 1.0f )
        {
            // Scale only if scale is not 1
            //D3DMatrixScale(&m_matTransform);
        }
         */

        if( m_fRotationX != 0.0f )
            modelViewMat = CreateRotationMatrix(MAT_ROT_X, m_fRotationX) * modelViewMat;
            
        if(m_fRotationY != 0.0f)
            modelViewMat = CreateRotationMatrix(MAT_ROT_Y, m_fRotationY) * modelViewMat;
        
        if(m_fRotationZ != 0.0f )
            modelViewMat = modelViewMat * CreateRotationMatrix(MAT_ROT_Z, m_fRotationZ) * modelViewMat;
         
        if(m_ptOrigin != point(0.0f, 0.0f, 0.0f))
           modelViewMat =  CreateTranslationMatrix(m_ptOrigin[0], m_ptOrigin[1], m_ptOrigin[2]) * modelViewMat;
         
        glUniformMatrix4fv(modelView, 1, GL_FALSE, (GLfloat *)(modelViewMat.transpose().matrix_entry));
        
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
        
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
        glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) (sizeof(float) * 3 * 3));
        
        glDrawElements(GL_TRIANGLES, sizeof(m_indicies)/sizeof(m_indicies[0]), GL_UNSIGNED_SHORT, 0);
        
        
        return R_SUCCESS;
    }
    
public:
    GLuint m_VertexBuffer;      // Vertex Buffer
    GLuint m_IndexBuffer;       // Index Buffer
};
