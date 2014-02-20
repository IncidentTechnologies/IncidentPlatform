// Light Object
// This is the dimension object for a light source

#pragma once

typedef enum dimension_light_type
{
    DIM_LIGHT_POINT,
    DIM_LIGHT_SPOT,
    DIM_LIGHT_DIRECTIONAL,
    DIM_LIGHT_AMBIENT           // For Ambient lights the only important parameter is the diffuse color
} DIMENSION_LIGHT_TYPE;

class light :
    public DimObject
{
public:
    light(point ptOrigin, DIMENSION_LIGHT_TYPE dlt) :
        DimObject(ptOrigin, DIM_LIGHT),
        m_dlt(dlt),
        m_colorDiffuse(color(1.0f, 1.0f, 1.0f, 1.0f)),
        m_Range(100.0f),
        m_ConstAttenuation(0.0f),
        m_InvAttenuation(0.125f),
        m_InvSqrAttenuation(0.0f),
        m_vDirection(vector(0.0f, -1.0f, 0.0f))
    {
        // Sets up a simple light pointing down
    }

    light(point ptOrigin, color colorDiffuse, DIMENSION_LIGHT_TYPE dlt) :
        DimObject(ptOrigin, DIM_LIGHT),
        m_dlt(dlt),
        m_colorDiffuse(colorDiffuse),
        m_Range(100.0f),
        m_ConstAttenuation(0.0f),
        m_InvAttenuation(0.125f),
        m_InvSqrAttenuation(0.0f),
        m_vDirection(vector(0.0f, -1.0f, 0.0f))
    {
        // Sets up a simple light pointing down
    }

    light(point ptOrigin, color colorDiffuse, vector vDirection, DIMENSION_LIGHT_TYPE dlt) :
        DimObject(ptOrigin, DIM_LIGHT),
        m_dlt(dlt),
        m_colorDiffuse(colorDiffuse),
        m_Range(100.0f),
        m_ConstAttenuation(0.0f),
        m_InvAttenuation(0.125f),
        m_InvSqrAttenuation(0.0f),
        m_vDirection(vDirection)
    {
        // Sets up a simple light pointing down
    }

    ~light()
    {
        
    }

    DIMENSION_LIGHT_TYPE GetLightType(){ return m_dlt; }
    color GetColor(){ return m_colorDiffuse; }

    const char *ToString() 
    {
        return "Light";
    }

    // Light object has no vertex info for now
    vertex* GetVertexBuffer()
    {
        return NULL;
    }
    int GetVertexCount()
    {
        return 0;
    }

protected:
    // Type of light
    DIMENSION_LIGHT_TYPE m_dlt;

    // Colors
    color m_colorDiffuse;

    // Shape
    float m_Range;
    float m_ConstAttenuation;
    float m_InvAttenuation;
    float m_InvSqrAttenuation;

    // Direction
    vector m_vDirection;
};