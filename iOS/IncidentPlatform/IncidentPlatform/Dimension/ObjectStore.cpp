#include "ObjectStore.h"

#include "DX9Translator.h"

ObjectStore::ObjectStore( DIMENSION_TYPE dt, DimensionImp* dimensionImp ) :
    m_MainCamera(NULL),
    m_plightAmbient(NULL),
    m_pDimensionImp(dimensionImp)
{
    RESULT res = R_SUCCESS;

    m_pObjectDB = new DimensionObjectDataBase;
    m_pLightDB = new LightDataBase;
    m_pBlitQueue = new BlitQueue;

    // Pass appropriate translator to the object store   
    /* LOOK: Perhaps pass this to a factory object at some point */
    switch(dt) {
            
#ifdef __WIN32__
        case DIMENSION_DX9: {
            DX9Translator *TempDX9Translator = new DX9Translator();
            m_DimTrans = dynamic_cast<DimensionTranslator*>(TempDX9Translator); 
            m_FlatTrans = dynamic_cast<FlatTranslator*>(TempDX9Translator);           // DX9Translator acts as both flat / dim translator
        } break;
#endif

        default: {
            res = R_ERROR; 
        } break;    
    }
}

ObjectStore::~ObjectStore() {
    if(m_pObjectDB != NULL)
        delete m_pObjectDB;
    if(m_pLightDB != NULL)
        delete m_pLightDB;
    if(m_pBlitQueue != NULL)
        delete m_pBlitQueue;
}

RESULT ObjectStore::AddObject( DimObject *d_obj, DimObject * &pObj )
{
    RESULT res = R_SUCCESS;
    DimObject *TranslatedObject = NULL;              

    res = m_DimTrans->TranslateObject(d_obj, TranslatedObject);

    if(TranslatedObject->GetObjectType() == DIM_LIGHT) {
        // Check for ambient light
        if(static_cast<light*>(TranslatedObject)->GetLightType() == DIM_LIGHT_AMBIENT) {
            if(m_plightAmbient == NULL)
            {
                m_plightAmbient = dynamic_cast<light*>(TranslatedObject);
            }
            else
            {
                printf("Duplicate ambient light created not inserting into Object Store\n");
                return R_ERROR;
            }
        }
    }

    // Check for camera
    if(TranslatedObject->GetObjectType() == DIM_CAMERA) {
        // If this is the first camera to be inserted this will
        // be our main camera
        if(m_MainCamera == NULL) {
            m_MainCamera = dynamic_cast<camera*>(TranslatedObject);
        }
        else {
            printf("Added another camera NOT IMPLEMENTED!!\n");
        }
    }

    if(res >= 0)  {
        pObj = TranslatedObject;
        return m_pObjectDB->Insert(TranslatedObject);
    }
    else {
        return res;
    }
}

RESULT ObjectStore::AddObject( FlatObject *d_obj, FlatObject* &pObj ) {
    RESULT r = R_SUCCESS;

    FlatObject *TranslatedObject = NULL;
    
    d_obj->ConvertToScreen(m_pDimensionImp->GetWidth(), m_pDimensionImp->GetHeight());

    r = m_FlatTrans->TranslateObject(d_obj, TranslatedObject);

    if(r >= 0 && TranslatedObject != NULL)
    {
        pObj = TranslatedObject;
        return m_pBlitQueue->Append(TranslatedObject);
    }

Error:
    return r;
}