#ifdef __WIN32__
#pragma once

// Common Private header for directX 9 objects

#define CUSTOMFVF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

#include <d3d9.h>
#include <d3dx9.h>

#include "RESULT.h"
#include "EHM.h"

// Translating the D3DCAPS9 into our own local enum
// since the D3DCAPS9 structure does not have a quick and
// easy look up function so we implement it for convenience
typedef enum DX9Capabilities
{
    DX9_CAPS_DEVICE_TYPE,                   // D3DDEVTYPE DeviceType
    DX9_CAPS_ADAPTER_ORDINAL,               // UINT AdapterOrdinal
    DX9_CAPS,                               // DWORD Caps
    DX9_CAPS2,                              // DWORD Caps2
    DX9_CAPS3,                              // DWORD Caps3
    DX9_CAPS_PRESENTATION_INTERVALS,        // DWORD PresentationIntervals
    DX9_CAPS_CURSOR_CAPS,                   // DWORD CursorCaps
    DX9_CAPS_DEV_CAPS,                      // DWORD DevCaps
    DX9_CAPS_PRIMITIVE_MISC,                // DWORD PrimitiveMiscCaps
    DX9_CAPS_RASTER,                        // DWORD RasterCaps
    DX9_CAPS_ZCMP,                          // DWORD ZCmpCaps
    DX9_CAPS_SRC_BLEND,                     // DWORD SrcBlendCaps
    DX9_CAPS_DEST_BLEND,                    // DWORD DestBlendCaps
    DX9_CAPS_ALPHA_CMP,                     // DWORD AlphaCmpCaps
    DX9_CAPS_SHADER,                        // DWORD ShadeCaps
    DX9_CAPS_TEXTURE,                       // DWORD TextureCaps
    DX9_CAPS_TEXTURE_FILTER,                // DWORD TextureFilterCaps
    DX9_CAPS_CUBE_TEXTURE_FILTER,           // DWORD CubeTextureFilterCaps
    DX9_CAPS_VOLUME_TEXTURE_FILTER,         // DWORD VolumeTextureFilterCaps
    DX9_CAPS_TEXTURE_ADDRESS,               // DWORD TextureAddressCaps
    DX9_CAPS_VOLUME_TEXTURE_ADDRESS,        // DWORD VolumeTextureAddressCaps
    DX9_CAPS_LINE,                          // DWORD LineCaps
    DX9_CAPS_MAX_TEXTURE_WIDTH,             // DWORD MaxTextureWidth
    DX9_CAPS_MAX_TEXTURE_HEIGHT,            // DWORD MaxTextureHeight
    DX9_CAPS_MAX_VOLUME_EXTENT,             // DWORD MaxVolumeExtent
    DX9_CAPS_MAX_TEXTURE_REPEAT,            // DWORD MaxTextureRepeat
    DX9_CAPS_MAX_TEXTURE_ASPECT_RATIO,      // DWORD MaxTextureAspectRatio
    DX9_CAPS_MAX_ANISOTROPY,                // DWORD MaxAnisotropy
    DX9_CAPS_MAX_VERTEX_W,                  // float MaxVertexW
    DX9_CAPS_GUARD_BAND_LEFT,               // float GuardBandLeft
    DX9_CAPS_GUARD_BAND_TOP,                // float GuardBandTop
    DX9_CAPS_GUARD_BAND_RIGHT,              // float GuardBandRight
    DX9_CAPS_GUARD_BAND_BOTTOM,             // float GuardBandBottom
    DX9_CAPS_EXTENTS_ADJUST,                // float ExtentsAdjust
    DX9_CAPS_STENCIL_CAPS,                  // DWORD StencilCaps
    DX9_CAPS_FVF,                           // DWORD FVFCaps
    DX9_CAPS_TEXTURE_OP,                    // DWORD TextureOpCaps
    DX9_CAPS_MAX_TEXTURE_BLEND_STAGES,      // DWORD MaxTextureBlendStages
    DX9_CAPS_MAX_SIMULTANEOUS_TEXTURES,     // DWORD MaxSimultaneousTextures
    DX9_CAPS_VERTEX_PROCESSING,             // DWORD VertexProcessingCaps
    DX9_CAPS_MAX_ACTIVE_LIGHTS,             // DWORD MaxActiveLights
    DX9_CAPS_MAX_USER_CLIP_PLANES,          // DWORD MaxUserClipPlanes
    DX9_CAPS_MAX_VERTEX_BLEND_MATRICES,     // DWORD MaxVertexBlendMatrices
    DX9_CAPS_MAX_VERTEX_BLEND_MATRIX_INDEX, // DWORD MaxVertexBlendMatrixIndex
    DX9_CAPS_MAX_POINT_SZIE,                // float MaxPointSize
    DX9_CAPS_MAX_PRIMITIVE_COUNT,           // DWORD MaxPrimitiveCount
    DX9_CAPS_MAX_VERTEX_INDEX,              // DWORD MaxVertexIndex
    DX9_CAPS_MAX_STREAMS,                   // DWORD MaxStreams
    DX9_CAPS_MAX_STREAM_STRIDE,             // DWORD MaxStreamStride
    DX9_CAPS_VERTEX_SHADER_VERSION,         // DWORD VertexShaderVersion
    DX9_CAPS_MAX_VERTEX_SHADER_CONST,       // DWORD MaxVertexShaderConst
    DX9_CAPS_PIXEL_SHADER_VERSION,          // DWORD PixelShaderVersion
    DX9_CAPS_PIXEL_SHADER_MAX_VALUE,        // float PixelShader1xMaxValue
    DX9_CAPS2_DEV,                          // DWORD DevCaps2
    DX9_CAPS_MASTER_ADAPTER_ORDINAL,        // UINT MasterAdapterOrdinal
    DX9_CAPS_ADAPTER_ORDINAL_IN_GROUP,      // UINT AdapterOrdinalInGroup
    DX9_CAPS_NUMBER_OF_ADAPTERS_IN_GROUP,   // UINT NumberOfAdaptersInGroup
    DX9_CAPS_DECL_TYPES,                    // DWORD DeclTypes
    DX9_CAPS_NUM_SIMULTANEOUS_RTS,          // DWORD NumSimultaneousRTs
    DX9_CAPS_STRETCH_RECT_FILTER_CAPS,      // DWORD StretchRectFilterCaps
    DX9_CAPS_VS20,                          // D3DVSHADERCAPS2_0 VS20Caps
    DX9_CAPS_D3DSHADER_2_0,                 // D3DPSHADERCAPS2_0 D3DPSHADERCAPS2_0
    DX9_CAPS_VERTEX_TEXTURE_FILTER,         // DWORD VertexTextureFilterCaps
    DX9_CAPS_MAX_VSHADER_INSTRUCTIONS_EXEC, // DWORD MaxVShaderInstructionsExecuted
    DX9_CAPS_MAX_PSHADER_INSTRUCTIONS_EXEC, // DWORD MaxPShaderInstructionsExecuted
    DX9_CAPS_MAX_VSHADER30_INSTRUCT_SLOTS,  // DWORD MaxVertexShader30InstructionSlots
    DX9_CAPS_MAX_PSHADER30_INSTRUCT_SLOTS,  // DWORD MaxPixelShader30InstructionSlots
    DX9_CAPS_RESERVED2,                     // DWORD Reserved2
    DX9_CAPS_RESERVED3,                     // DWORD Reserved3
} DX9_CAPABILITIES;

// Helper Functions
/*
RESULT PrintDX9Capability(IDirect3DDevice9 *pDevice, DX9_CAPABILITIES dx9cap)
{
    RESULT r = R_NOT_IMPLEMENTED;
    
    /*
    D3DCAPS9 DX9Caps;
    
    pDevice->GetDeviceCaps(&DX9Caps);


    // TODO : DO IT

    
Error:
    return r;
}
*/

#endif // #ifdef __WIN32__
