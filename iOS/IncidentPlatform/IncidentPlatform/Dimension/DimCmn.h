#pragma once

// Supported Dimension Implementation types

typedef enum Dimension_Types
{
    DIMENSION_DEFAULT,          // Default set at compile time (likely software)
    DIMENSION_DX9,              // DirectX 9
    DIMENSION_DX10,             // DirectX 10
    DIMENSION_GL200,            // OpenGL 2.0
    DIMENSION_SOFT              // Software based rendering
} DIMENSION_TYPE;