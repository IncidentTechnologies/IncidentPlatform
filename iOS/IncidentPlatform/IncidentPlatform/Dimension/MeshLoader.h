#pragma once

// MeshLoader is a class that will input a filename and a fileformat and it will in turn
// output a mesh (dimension format) which can then be manipulated by the engine.

#include "EHM.h"

//#include "MeshMd3.h"
#include "MeshObj.h"

using namespace dss;

typedef enum MeshType {
    MESH_MD3,
    MESH_OBJ,
    MESH_INVALID
} MESH_TYPE;

class MeshLoader {
public:
    MeshLoader() {
        /* empty */
    }

    ~MeshLoader() {
        /* empty */
    }


    RESULT LoadMesh(char *pszFilename, MESH_TYPE mt, DimObject* &n_RetObj) {
        RESULT r = R_SUCCESS;

        switch(mt) {
            case MESH_MD3: {
                n_RetObj = new MeshMd3();
                dynamic_cast<MeshMd3*>(n_RetObj)->LoadMesh();
            } break;
                
            case MESH_OBJ: {
                n_RetObj = new MeshObj(pszFilename);
            } break;

            case MESH_INVALID:    
            default: {
                printf("Mesh Type %d not supported\n");
                n_RetObj = NULL;
                return R_FAIL;
            } break;
        }

Error:
        return r;
    }

private:

};