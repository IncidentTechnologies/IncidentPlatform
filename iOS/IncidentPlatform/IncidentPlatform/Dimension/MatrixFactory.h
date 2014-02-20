#pragma once

// Matrix Factory to produce different kinds of matricies

#include "matrix.h"
#include <string.h>
#include <math.h>

typedef enum Matrix4Type {
    MATRIX_PROJECTION,
    MATRIX_INVALID
} MATRIX4_TYPE;

typedef enum MatrixRotationType {
    MAT_ROT_X,
    MAT_ROT_Y,
    MAT_ROT_Z,
    MAT_ROT_INVALID
} MAT_ROT_TYPE;

matrix<float> CreateProjectionMatrix(float fov, float aspect, float near, float far);
matrix<float> CreateOpenGLProjectionMatrix(float near, float far);


matrix<float> CreateTranslationMatrix(float x, float y, float z);
matrix<float> CreateRotationMatrix(MAT_ROT_TYPE type, float theta);
matrix<float> CreateScaleMatrix(float x, float y, float z);
matrix<float> CreateIdentity(int N);

class MatrixFactory {
    MatrixFactory(){/*empty stub*/}
    
    ~MatrixFactory(){/*empty stub*/}
    
    /*
    static matrix<float> MakeMatrix(MATRIX4_TYPE type, ...) {
        switch(type) {
            case
        }
    }
    */
    
};

// left right, top bottom are -1.0f, 1.0f and -1.0f, 1.0f
matrix<float> CreateOpenGLProjectionMatrix(float near, float far) {
    matrix<float> mat = CreateIdentity(4);
    
    // XY projection
    mat[0][0] = near;
    mat[1][1] = near;
    
    // Clipping in Z
    mat[2][2] = (-1.0f * (near + far)) / (far - near);
    mat[2][3] = (-2.0f * far * near) / (far - near);
    
    mat[3][2] = -1.0f;
    
    return mat;
}

matrix<float> CreateProjectionMatrix(float fov, float aspect, float near, float far) {
    matrix<float> mat = CreateIdentity(4);
    
    const float tanHalfFov = tanf((fov * PI)/360.0f);
    
    // XY projection
    mat[0][0] = 1.0f / (tanHalfFov * aspect);
    mat[1][1] = 1.0f / tanHalfFov;

    // Clipping in Z
    mat[2][2] = ((-1.0f * near) - far) / (near - far);
    mat[2][3] = (2.0f * far * near) / (near - far);

    mat[3][2] = 1.0f;
    
    return mat;
}

matrix<float> CreateTranslationMatrix(float x, float y, float z){
    matrix<float> mat = CreateIdentity(4);
    
    mat[0][3] = x;
    mat[1][3] = y;
    mat[2][3] = z;
    
    return mat;
}

matrix<float> CreateScaleMatrix(float x, float y, float z) {
    matrix<float> mat = CreateIdentity(4);
    
    mat[0][0] = x;
    mat[1][1] = y;
    mat[2][2] = z;
    
    return mat;
}

matrix<float> CreateRotationMatrix(MAT_ROT_TYPE type, float theta) {
    matrix<float> mat = CreateIdentity(4);
    
    switch(type) {
        case MAT_ROT_X: {
            mat[1][1] = cosf(theta);
            mat[1][2] = -1.0f * sinf(theta);
            mat[2][1] = sinf(theta);
            mat[2][2] = cosf(theta);
        } break;
        
        case MAT_ROT_Y: {
            mat[0][0] = cosf(theta);
            mat[0][2] = sinf(theta);
            mat[2][0] = -1.0f * sinf(theta);
            mat[2][2] = cosf(theta);
        } break;
            
        case MAT_ROT_Z: {
            mat[0][0] = cosf(theta);
            mat[0][1] = -1.0f * sinf(theta);
            mat[1][0] = sinf(theta);
            mat[1][1] = cosf(theta);
        } break;
            
        default: break;
    }
    
    return mat;
}

matrix<float> CreateIdentity(int N){
    matrix<float> mat(N, N);
    mat.clear();
    
    for(int i = 0; i < N; i++)
        mat[i][i] = 1.0f;
    
    return mat;
}