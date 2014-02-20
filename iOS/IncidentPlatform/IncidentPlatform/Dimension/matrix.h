// Note: point is not capitalized to try and distinguish it as a normal
// data type rather than a specialized one.

// template based N x M Matrix
// convention i, j indexing

#pragma once

#include "cmnPriv.h"
#include <stdlib.h>

template <class T=float>
class matrix {
public:
    // constructors
    matrix(int N, int M) :
        N(N),
        M(M),
        matrix_entry(NULL)
    {
        init();
    }
    
    void init() {
        //matrix_entry = new T[N * M];
        matrix_entry = (T*)malloc(sizeof(T)* N * M);
    }
    
    /*
    ~matrix() {
        if(matrix_entry != NULL) {
            free(matrix_entry);
            matrix_entry = NULL;
        }
    }
     */
    
    matrix<T> transpose() {
        matrix<T> transposeMatrix(M, N);
        for(int i = 0; i < M; i++)
            for(int j = 0; j < N; j++)
                transposeMatrix[i][j] = getVal(j, i);
        
        return transposeMatrix;
    }
    
    // Prints Memory to Console
    void PrintMem() {
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < M; j++)
                printf("%f ", getVal(i, j));
            
            printf("\n");
        }
        
        printf("\n");
    }
    
    void ToString() {
        printf("%i x %i Matrix\n", N, M);
    }
    
    void clear() {
        memset(matrix_entry, 0, (sizeof(T) * N * M));
    }
    
    class matrixRow {
        matrix<T> &parent;
        int x;
        public:
            matrixRow(matrix<T>& p, int X) : parent(p), x(X) {/*stub*/}
            T &operator[](int y) const { return parent.getVal(x, y); }
    };
    
    T &getVal(int x, int y){
        return matrix_entry[(x * M) + y];
    }
    matrixRow operator[](int x) { return matrixRow(*this, x); }
    
    matrix operator* (matrix mat) {
        matrix<T> prodMat(N, mat.M);
        prodMat.clear();
        
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < mat.M; j++) {
                prodMat[i][j] = (T)0;
                for(int k = 0; k < M; k++) {
                    prodMat[i][j] += getVal(i, k) * mat.getVal(k, j);
                }
            }
        }
        
        return prodMat;
    }
    
public:
    // matrix data
    T *matrix_entry;
    int N;
    int M;

private:
    friend class matrixRow;
};