#ifdef __WIN32__
#include "DX9Translator.h"

RESULT DX9Translator::TranslateObject( FlatObject* &d_foIn, FlatObject* &n_foOut )
{
    RESULT r = R_OK;

    switch(d_foIn->GetObjectType())
    {
        case FLAT_BLIT:
        {                
            n_foOut = new DX9Blit(d_foIn);
        } break;

        case FLAT_TEXT:
        {
            n_foOut = new DX9Text(d_foIn);
        } break;

        case FLAT_LINE:
        {
            n_foOut = new DX9Line(d_foIn);
        } break;

        case FLAT_COMMON_CONTROL:
        {
            // We are not creating anything new here so we simply call the functions correctly
            // and reassign the pointers (but don't delete!)
            CRM(dynamic_cast<IComposite*>(d_foIn)->Translate((FlatTranslator*)this), "Failed to convert common control:%s", d_foIn->ToString());
            n_foOut = d_foIn;
            d_foIn = NULL;      // keep away from caller function
            return r;           // prevent delete of controller object
        } break; 

        default:
        {
            printf("Tried to add a flat object that's not supported: %s\n", d_foIn->ToString()); 
        } break;
    }

    delete d_foIn;
    d_foIn = NULL;

Error:
    return (n_foOut == NULL) ? R_ERROR : R_OK;
}

RESULT DX9Translator::TranslateObject( DimObject * &d_doIn, DimObject * &n_doOut )
{
    RESULT res = R_OK;

    switch(d_doIn->GetObjectType())
    {
    case DIM_TRIANGLE:  
        {
            n_doOut = new DX9Triangle(d_doIn);
        } break;

    case DIM_VOLUME:    
        {
            n_doOut = new DX9Volume(d_doIn);                              
        } break;

    case DIM_SPHERE:    
        {
            n_doOut = new DX9Sphere(d_doIn);                            
        } break;

    case DIM_CAMERA:    
        {
            n_doOut = new DX9Camera(d_doIn);
        } break;

    case DIM_LIGHT:     
        { 
            n_doOut = new DX9Light(d_doIn);
        } break;

        /*case DIM_TEXT:	
        {
        n_doOut = new DX9Text(d_doIn);
        } break;*/

    case DIM_TEXT3D:   
        { 
            n_doOut = new DX93DText(d_doIn);
        } break;

    case DIM_RECTANGLE:
        {
            n_doOut = new DX9Rectangle(d_doIn);
        } break;

    case DIM_MESH_MD3:
        {
            n_doOut = new DX9MeshMd3(d_doIn);
        } break;

    case DIM_HTERRAIN:
        {
            n_doOut = new DX9HTerrain(d_doIn);
        } break;

    case DIM_PLANE:
        {
            n_doOut = new DX9Plane(d_doIn);
        } break;

    default:    
        {
            printf("Tried to add a dimension object that's not supported: %s\n", d_doIn->ToString()); 
        } break;
    }

    res = (n_doOut == NULL) ? R_ERROR : R_OK;

    delete d_doIn;
    d_doIn = NULL;

Error:
    return res;
}

#endif // #ifdef __WIN32__
