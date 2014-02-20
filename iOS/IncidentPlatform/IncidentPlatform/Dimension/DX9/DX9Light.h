#ifdef __WIN32__
// DirectX9 implementation of light

#pragma once

class DX9Light :
    public light,
    public DX9DimObject
{
private:
    RESULT InitDX9Light()
    {
        ZeroMemory(&m_d3dlight, sizeof(m_d3dlight));
        return R_OK;
    }

public:
    DX9Light(point ptOrigin, DIMENSION_LIGHT_TYPE dlt) :
        light(ptOrigin, dlt)
    {
        RESULT res = InitDX9Light();
    }

    DX9Light(DimObject *obj) :
        light(obj->m_ptOrigin, static_cast<light*>(obj)->GetColor(),
                               static_cast<light*>(obj)->GetLightType())
    {
        RESULT res = InitDX9Light();
    }

    HRESULT DX9Init(LPDIRECT3DDEVICE9 pd3device9)
    {
        HRESULT hr = S_OK;

        // Enable lighting 
        // This call is called as many times as there are lights
        // which is an inefficiency but is ok
        /* INEFFICIENCY */
        pd3device9->SetRenderState(D3DRS_LIGHTING, TRUE);

        if(m_dlt == DIM_LIGHT_AMBIENT)
        {
            // For an ambient light we simply need to set the ambient
            // levels for the render state
            pd3device9->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(cbR(m_colorDiffuse), 
                                                                    cbG(m_colorDiffuse),
                                                                    cbB(m_colorDiffuse)));

            /* NOTE: Eventually each object will have it's own material but we need
                     to set up a global material here meanwhile.  This code needs to be
                     revised at some point */
            D3DMATERIAL9 materialGlobal;    // create the material struct

            ZeroMemory(&materialGlobal, sizeof(D3DMATERIAL9));    // clear out the struct for use
            materialGlobal.Diffuse.r = materialGlobal.Ambient.r = 1.0f;    // set the material to full red
            materialGlobal.Diffuse.g = materialGlobal.Ambient.g = 1.0f;    // set the material to full green
            materialGlobal.Diffuse.b = materialGlobal.Ambient.b = 1.0f;    // set the material to full blue
            materialGlobal.Diffuse.a = materialGlobal.Ambient.a = 1.0f;    // set the material to full alpha

            pd3device9->SetMaterial(&materialGlobal);    // set the globaly-used material to &material
        }
        else
        {
            // Set up light        
            switch(m_dlt)
            {
            case DIM_LIGHT_POINT:           m_d3dlight.Type = D3DLIGHT_POINT;
                                            break;

            case DIM_LIGHT_SPOT:            m_d3dlight.Type = D3DLIGHT_SPOT;
                                            break;

            case DIM_LIGHT_DIRECTIONAL:     m_d3dlight.Type = D3DLIGHT_DIRECTIONAL;
                                            break;

            default:                        m_d3dlight.Type = D3DLIGHT_POINT;
                                            break;                                 
            }

            // Set up the light correctly
            // (don't need to do this for ambient lights!)
            hr = DX9Update();

            // Turn on the light (Set ID)
            hr = pd3device9->SetLight(this->GetID(), &m_d3dlight);
            hr = pd3device9->LightEnable(this->GetID(), true);            
        }
        return hr;
    }

    HRESULT DX9Update()
    {
        HRESULT hr = S_OK;
        
        // All the update requires is that we apply the
        // DimObject transformations to the light (rotation, translation etc)
        // We also need to add color manipulations, effectively we are syncing up
        // the two data structures in light and D3DLIGHT

        // Color
        m_d3dlight.Diffuse.a = cfA(m_colorDiffuse);
        m_d3dlight.Diffuse.r = cfR(m_colorDiffuse);
        m_d3dlight.Diffuse.g = cfG(m_colorDiffuse);
        m_d3dlight.Diffuse.b = cfB(m_colorDiffuse);

        // Shape
        m_d3dlight.Range = m_Range;
        m_d3dlight.Attenuation0 = m_ConstAttenuation;
        m_d3dlight.Attenuation1 = m_InvAttenuation;
        m_d3dlight.Attenuation2 = m_InvSqrAttenuation;

        // Position
        D3DVECTOR vPosition = {pX(m_ptOrigin), pY(m_ptOrigin), pZ(m_ptOrigin)};
        m_d3dlight.Position = vPosition;

        // Direction
        // We should apply transformations here if needed to m_vDirection

        D3DVECTOR vDirection = {vX(m_vDirection), vY(m_vDirection), vZ(m_vDirection)};
        m_d3dlight.Direction = vDirection;        

        return hr;
    }

    RESULT Render(DimensionImpDX9* pDimImpDX9)
    {
        return R_NOT_IMPLEMENTED;
    }

    RESULT Render(LPDIRECT3DDEVICE9 pd3device9)
    {
        RESULT r = R_OK;
        
        HRESULT hr = S_OK;
        
        CHRM(DX9Update(), "DX9Light::DX9Update");
        CHRM(pd3device9->SetLight(this->GetID(), &m_d3dlight), "DX9Light::SetLight");
        CHRM(pd3device9->LightEnable(this->GetID(), true), "DX9Light::LightEnable");

Error:
        return r;
    }

private:
    D3DLIGHT9 m_d3dlight;
};

#endif // #ifdef __WIN32__
