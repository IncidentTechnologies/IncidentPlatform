// Note: vector is not capitalized to try and distinguish it as a normal
// data type rather than a specialized one.

#pragma once

#include <math.h>
#include <stdio.h>
#include <string.h>

#define VECTOR_DIM 3

#define DIM_X 0
#define DIM_Y 1
#define DIM_Z 2
#define DIM_A 3

#define vX(v) v.tuple[DIM_X]
#define vY(v) v.tuple[DIM_Y]
#define vZ(v) v.tuple[DIM_Z]

#define vXYZ(v) v.tuple[DIM_X], v.tuple[DIM_Y], v.tuple[DIM_Z]


#define tX(v) v->tuple[DIM_X]
#define tY(v) v->tuple[DIM_Y]
#define tZ(v) v->tuple[DIM_Z]

#define tXYZ(v) v->tuple[DIM_X], v->tuple[DIM_Y], v->tuple[DIM_Z]

class vector
{
public:
    // Constructors
    vector() 
    {
        memset(tuple, 0, sizeof(tuple));
    }

    ~vector()
    {
        /*empty stub*/
    }
    
    vector(float Tuple[VECTOR_DIM]) 
    {
        for(int i = 0; i < VECTOR_DIM; i++)
        {
            tuple[i] = Tuple[i];
        }
    }

    vector(float x, float y, float z) 
    {
        tuple[0]=x;
        tuple[1]=y;
        tuple[2]=z;
        //tuple[3]=0;
    }
    
    vector(float x, float y, float z, float w) 
    {
        tuple[0]=x;
        tuple[1]=y;
        tuple[2]=z;
        //tuple[3]=w;
    }

    void set(float x, float y, float z)
    {
        tuple[0]=x;
        tuple[1]=y;
        tuple[2]=z;
        //tuple[3]=0;
    }

    void set(float x, float y, float z, float w)
    {
        tuple[0]=x;
        tuple[1]=y;
        tuple[2]=z;
        //tuple[3]=w;
    }

    void set(vector v)
    {
        for(int i = 0; i < VECTOR_DIM; i++)
        {
            tuple[i] = v.tuple[i];
        }
    }

    float magnitude()
    {
        return sqrt(tuple[0]*tuple[0] + 
                    tuple[1]*tuple[1] +
                    tuple[2]*tuple[2]);
    }

    static float magnitude(vector v)
    {
        return v.magnitude();
    }

    void normalize()
    {
        float fMag = magnitude();

        tuple[0] = tuple[0] / fMag;
        tuple[1] = tuple[1] / fMag;
        tuple[2] = tuple[2] / fMag;
    }

    // Equality Operator
    vector& operator= (const vector &v)
    {
        tuple[0] = v.tuple[0];
        tuple[1] = v.tuple[1];
        tuple[2] = v.tuple[2];

        return *this;
    }
    
    // Arithmetic Operators
    vector operator+ (vector v) 
    {
        return vector(tuple[0]+v.tuple[0], 
                      tuple[1]+v.tuple[1], 
                      tuple[2]+v.tuple[2]);
                      //tuple[3]+v.tuple[3]);
    }
    
    vector operator- (vector v) 
    {
        return vector(tuple[0]-v.tuple[0], 
                      tuple[1]-v.tuple[1], 
                      tuple[2]-v.tuple[2]);
                      //tuple[3]-v.tuple[3]);
    }

    // Scalar Multiplication
    vector operator* (float c) 
    {
        return vector(tuple[0]*c, 
                      tuple[1]*c, 
                      tuple[2]*c); 
                      //tuple[3]*c);
    }
    
    friend vector operator* (float c, vector v) 
    {
        return vector(c*v.tuple[0], 
                      c*v.tuple[1], 
                      c*v.tuple[2]); 
                      //c*v.tuple[3]);
    };

    // Dot Product
    float operator* (vector v) 
    {	
        return ((tuple[0]*v.tuple[0]) +
                (tuple[1]*v.tuple[1]) +
                (tuple[2]*v.tuple[2]));
                //(tuple[3]*v.tuple[3]) );	
    }

    // Cross Product
    // Using % makes sense since it's the closest thing to a cross product operator
    vector operator% (vector v) 
    {	
        return vector((tuple[1]*v.tuple[2] - tuple[2]*v.tuple[1]),
                      (tuple[2]*v.tuple[0] - tuple[0]*v.tuple[2]),
                      (tuple[0]*v.tuple[1] - tuple[1]*v.tuple[0]));
                      //(1));	
    }

    void VRotate(vector V, float angle)
    {
        tuple[DIM_X] = (vX(V)*(*this*V) + 
                         (tX(this)*(vY(V)*vY(V) + vZ(V)*vZ(V)) - vX(V)*(vY(V)*tY(this)+vZ(V)*tZ(this)))*cos(angle) + 
                         V.magnitude()*(vY(V)*tZ(this) - vZ(V)*tY(this))*sin(angle)) / 
                         (V.magnitude()*V.magnitude());
        
        tuple[DIM_Y] = (vY(V)*(*this*V) + 
                         (tY(this)*(vX(V)*vX(V) + vZ(V)*vZ(V)) - vY(V)*(vX(V)*tX(this)+vZ(V)*tZ(this)))*cos(angle) + 
                         V.magnitude()*(vZ(V)*tX(this) - vX(V)*tZ(this))*sin(angle)) / 
                         (V.magnitude()*V.magnitude());

        tuple[DIM_Z] = (vZ(V)*(*this*V) + 
                         (tZ(this)*(vX(V)*vX(V) + vY(V)*vY(V)) - vZ(V)*(vX(V)*tX(this)+vY(V)*tY(this)))*cos(angle) + 
                         V.magnitude()*(vX(V)*tY(this) - vY(V)*tX(this))*sin(angle)) / 
                         (V.magnitude()*V.magnitude());
    }
    
    float &operator[](int x) {
        return tuple[x];
    }

    // Function is Inline due to it's standard nature
    void ToString() 
    {
        printf("vector Class: { x: %f y: %f z: %f w: %f }\n", tuple[0], tuple[1], tuple[2]/*, tuple[3]*/);
    }

//private:
    float tuple[VECTOR_DIM]; //tuple[3] == 0 always
};

/*vector VectorRotate(vector P, vector V, float angle)
{
    vector p;

    float dotProd = P*V;
    float vMag = V.magnitude();
    
    p.tuple[DIM_X] = (vX(V)*dotProd + 
                     (vX(P)*(vY(V)*vY(V) + vZ(V)*vZ(V)) - vX(V)*(vY(V)*vY(P)+vZ(V)*vZ(P)))*cos(angle) + 
                     vMag*(vY(V)*vZ(P) - vZ(V)*vY(P))*sin(angle)) / 
                     (vMag*vMag);
    
    p.tuple[DIM_Y] = (vY(V)*dotProd + 
                     (vY(P)*(vX(V)*vX(V) + vZ(V)*vZ(V)) - vY(V)*(vX(V)*vX(P)+vZ(V)*vZ(P)))*cos(angle) + 
                     vMag*(vZ(V)*vX(P) - vX(V)*vZ(P))*sin(angle)) / 
                     (vMag*vMag);

    p.tuple[DIM_Z] = (vZ(V)*dotProd + 
                     (vZ(P)*(vX(V)*vX(V) + vY(V)*vY(V)) - vZ(V)*(vX(V)*vX(P)+vY(V)*vY(P)))*cos(angle) + 
                     vMag*(vX(V)*vY(P) - vY(V)*vX(P))*sin(angle)) / 
                     (vMag*vMag);

    return p;
}*/
