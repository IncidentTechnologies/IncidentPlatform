// A Transformation Matrix for use with translation, shear, rotation and scaling

#pragma once

#include "matrix.h"

#define TRANSLATION_DIM 4
#define ROTATION_DIM 4
#define SHEAR_DIM 4
#define SCALE_DIM 4

typedef matrix<float> Matrix4x4f;

class TransformationMatrix : 
    public Matrix4x4f
{
public:
    
private:
    float translationTuple[TRANSLATION_DIM];
    float rotationTuple[ROTATION_DIM];
    float shearTuple[SHEAR_DIM];
    float scaleTuple[SCALE_DIM];
};

typedef TransformationMatrix TMat;