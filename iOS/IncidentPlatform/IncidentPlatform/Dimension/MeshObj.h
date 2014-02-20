#pragma once

// MeshObj will create a mesh from a OBJ file

#include "DimObject.h"
#include "vertex.h"
#include "RESULT.h"
#include "EHM.h"

#define OBJ_MAX_PATH 256

class MeshObj :
    public DimObject
{
public:
    MeshObj() :
      DimObject(DIM_MESH_MD3),
      m_pszFilepath(NULL)
    {/*empty stub*/}

    MeshObj(char *pszFilepath) :
        DimObject(DIM_MESH_MD3),
        m_pszFilepath(pszFilepath)
    {
        LoadMesh();
    }

    ~MeshObj() {
        /* empty */
    }

    RESULT LoadMesh();
    
    char* GetFilepath(){ return m_pszFilepath; }
    vertex* GetVertexBuffer() { return m_verts; }
    int GetVertexCount() { return m_indicies_n; }
    const char *ToString() { return "OBJ Mesh"; }

private:
    char *m_pszFilepath;

public:
    vertex *m_verts;
    int m_verts_n;
    
    unsigned short *m_indicies;
    int m_indicies_n;
    
};