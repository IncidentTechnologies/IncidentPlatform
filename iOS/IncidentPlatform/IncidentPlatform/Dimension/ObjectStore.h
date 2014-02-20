// This object store is a container object for the object database
// and will be held by Dimension and manipulated with nuke

#pragma once

//#include "btree.hxx"        // Use a binary tree as a container for now
#include "kdtree.h"

#include "DimensionPrimitives.h"
//#include "DX9Primitives.h"
#include "DimCmn.h"

#include "DimensionTranslator.h"
#include "FlatTranslator.h"

#include "DimensionImp.h"

//typedef btree<DimObject*> ObjectDataBase; 
typedef kdtree DimensionObjectDataBase;
typedef list<DimObject*> LightDataBase;
typedef list<FlatObject*> BlitQueue;
typedef list<DimObject*> CameraDataBase;

class ObjectStore {
public:
    ObjectStore(DIMENSION_TYPE dt, DimensionImp *dimensionImp);

    ~ObjectStore();

    RESULT AddObject(DimObject *d_obj, DimObject * &pObj);
    RESULT AddObject(FlatObject *d_obj, FlatObject* &pObj);

    /* LOOK: Do Not Support Delete yet
    RESULT DeleteObject(DimObject *obj)
    { 
        return m_pObjectDB->Delete(obj); 
    }*/

    // Object Support
    DimensionObjectDataBase *GetDimensionObjectDataBase(){ return m_pObjectDB; }

    // Lighting Manager 
    LightDataBase *GetLightDataBase(){ return m_pLightDB; }
    light *GetAmbientLight(){ return m_plightAmbient; }
    RESULT SetAmbientLight(light *NewAmbientLight){ m_plightAmbient = NewAmbientLight; return R_SUCCESS; }

    // Camera Support
    CameraDataBase  *GetCameraDB(){ return m_pCameraDB; }
    camera *GetMainCamera(){ return m_MainCamera; }
    RESULT SetMainCamera(camera *NewCam){ m_MainCamera = NewCam; return R_SUCCESS; }

    // Blit Management
    BlitQueue *GetBlitQueue(){ return m_pBlitQueue; }

    // Material Manager (Textures Etc)

private:
    // Our Resident Dimension Translator
    DimensionTranslator *m_DimTrans;

    // Flat Translator (can point to the same translator)
    FlatTranslator *m_FlatTrans;

    // Objects
    DimensionObjectDataBase *m_pObjectDB;

    // Lights
    LightDataBase *m_pLightDB;
    light *m_plightAmbient;

    // Blit Queue
    BlitQueue *m_pBlitQueue;

    // Camera List
    CameraDataBase *m_pCameraDB;
    camera *m_MainCamera;

    /*TODO: Material management here!*/

    DimensionImp *m_pDimensionImp;
};