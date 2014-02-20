#include "MeshObj.h"
#include <math.h>
#include "dss_list.h"

using namespace dss;

RESULT MeshObj::LoadMesh() {
    RESULT r = R_SUCCESS;

    FILE *file;
    file = fopen(m_pszFilepath, "rb");
    if(file == NULL) {
        printf("Could not open %s\n", m_pszFilepath);
        //CBRM(0, "");
        return R_ERROR;
    }
    
    list<point*> verts;
    list<vector*> normals;
    list<point2*> UVs;
    list<unsigned short> indexVert, indexUV, indexNormal;

    while(1) {
        char objLine[OBJ_MAX_PATH];
        int res = fscanf(file, "%s", objLine);
        if(res == EOF) {
            break;
        }
        
        if(strcmp(objLine, "v") == 0) {
            point *tempVert = new point();
            fscanf(file, "%f %f %f\n", &tempVert->tuple[0], &tempVert->tuple[1], &tempVert->tuple[2]);
            verts.Append(tempVert);
        } else if(strcmp(objLine, "vt") == 0) {
            point2 *tempUV = new point2();
            fscanf(file, "%f %f\n", &tempUV->tuple[0], &tempUV->tuple[1]);
            UVs.Append(tempUV);
        } else if(strcmp(objLine, "vn") == 0) {
            vector *tempNormal = new vector();
            fscanf(file, "%f %f %f\n", &tempNormal->tuple[0], &tempNormal->tuple[1], &tempNormal->tuple[2]);
            normals.Append(tempNormal);
        } else if(strcmp(objLine, "f") == 0) {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]
            );
            
            if (matches != 9){
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return R_ERROR;
            }
            
            for(int i = 0; i < 3; i++) {
                indexVert.Append((unsigned short)(vertexIndex[i] - 1)); // OBJ indexing is one-based
                indexUV.Append((unsigned short)(uvIndex[i] - 1));
                indexNormal.Append((unsigned short)(normalIndex[i] - 1));
            }
        }
    }
    
    m_verts_n = verts.length();
    m_verts = new vertex[m_verts_n];
    int i = 0;
    for(list<point*>::iterator it = verts.First(); it != NULL; it++) {
        m_verts[i].set(*(*it));
        i++;
    }
    
    i = 0;
    m_indicies_n = indexVert.length();
    m_indicies = new unsigned short[m_indicies_n];
    for(list<unsigned short>::iterator it = indexVert.First(); it != NULL; it++) {
        m_indicies[i] = (*it);
        m_verts[m_indicies[i]].ToString();
        i++;
    }

Error:
    return r;
}
