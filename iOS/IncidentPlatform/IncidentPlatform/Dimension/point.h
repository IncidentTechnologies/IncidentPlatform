#pragma once

// Note: point is not capitalized to try and distinguish it as a normal
// data type rather than a specialized one.

#include "vector.h"

#define POINT_DIM 3

#define DIM_X 0
#define DIM_Y 1
#define DIM_Z 2
#define DIM_A 3

#define pX(p) p.tuple[DIM_X]
#define pY(p) p.tuple[DIM_Y]
#define pZ(p) p.tuple[DIM_Z]
#define pA(p) p.tuple[DIM_A]

#define pXYZ(p) p.tuple[DIM_X], p.tuple[DIM_Y], p.tuple[DIM_Z]


class point
{
public:
    // Constructors
    point() 
    {
        memset(tuple, 0, sizeof(tuple));
    }

    ~point()
    {
        /*empty stub*/
    }
    
    point(float Tuple[POINT_DIM]) 
    {
        for(int i = 0; i < POINT_DIM; i++)
        {
            tuple[i] = Tuple[i];
        }
    }

    point(vector v)
    {
        tuple[DIM_X] = v.tuple[DIM_X];
        tuple[DIM_Y] = v.tuple[DIM_Y];
        tuple[DIM_Z] = v.tuple[DIM_Z];
    }

    point(float x, float y)
    {
        tuple[0] = x;
        tuple[1] = y;
        tuple[2] = 0.0f;
    }

    point(float x, float y, float z) 
    {
        tuple[0]=x;
        tuple[1]=y;
        tuple[2]=z;
        //tuple[3]=1;
    }
    
    point(float x, float y, float z, float w) 
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
    }

    void set(float x, float y, float z, float w)
    {
        tuple[0] = x;
        tuple[1] = y;
        tuple[2] = z;
        tuple[3] = w;
    }

    void set(point p)
    {
        for(int i = 0; i < POINT_DIM; i++)
        {
            tuple[i] = p.tuple[i];
        }
    }

    // Equality Operator
    point& operator= (const point &p)
    {
        tuple[0] = p.tuple[0];
        tuple[1] = p.tuple[1];
        tuple[2] = p.tuple[2];
        
        return *this;
    }
    
    // Arithmetic Operators
    point operator+ (point q) 
    {
        return point(tuple[0]+q.tuple[0], 
                     tuple[1]+q.tuple[1], 
                     tuple[2]+q.tuple[2]); 
                     //tuple[3]+q.tuple[3]);
    }

    point operator+ (vector v)
    {
        return point(tuple[0]+v.tuple[0], 
                     tuple[1]+v.tuple[1], 
                     tuple[2]+v.tuple[2]);
    }

    const point &operator+=(const point &q)
    {
        tuple[0]+=q.tuple[0];
        tuple[1]+=q.tuple[1];
        tuple[2]+=q.tuple[2];
        
        return *this;
    }
    
    const point &operator+=(const vector &v)
    {
        tuple[0]+=v.tuple[0];
        tuple[1]+=v.tuple[1];
        tuple[2]+=v.tuple[2];
        
        return *this;
    }

    vector operator- (point q) 
    {
        return vector(tuple[0]-q.tuple[0], 
                      tuple[1]-q.tuple[1], 
                      tuple[2]-q.tuple[2]);
                      //tuple[3]-q.tuple[3]);
    }

    // Scalar Multiplication
    point operator* (float c) 
    {
        return point(tuple[0]*c, 
                     tuple[1]*c, 
                     tuple[2]*c);
                     //tuple[3]*c);
    }

    bool operator== (point q)
    {
        return( (tuple[0] == q.tuple[0]) && 
                (tuple[1] == q.tuple[1]) && 
                (tuple[2] == q.tuple[2]));
    }

    bool operator!= (point q)
    {
        return( (tuple[0] != q.tuple[0]) || 
                (tuple[1] != q.tuple[1]) || 
                (tuple[2] != q.tuple[2]));
    }
    
    friend point operator* (float c, point q) 
    {
        return point(c*q.tuple[0], 
                     c*q.tuple[1], 
                     c*q.tuple[2]); 
                     //c*q.tuple[3]);
    };

    // Dot Product
    float operator* (point q) 
    {	
        return ((tuple[0]*q.tuple[0]) +
                (tuple[1]*q.tuple[1]) +
                (tuple[2]*q.tuple[2]));
                //(tuple[3]*q.tuple[3]));	
    }

    float operator* (vector v) 
    {	
        return ((tuple[0]*v.tuple[0]) +
                (tuple[1]*v.tuple[1]) +
                (tuple[2]*v.tuple[2]));
                //(tuple[3]*q.tuple[3]));	
    }

    // Cross Product
    // Using % makes sense since it's the closest thing to a cross product operator
    point operator% (point q) 
    {	
        return point((tuple[1]*q.tuple[2] - tuple[2]*q.tuple[1]),
                     (tuple[2]*q.tuple[0] - tuple[0]*q.tuple[2]),
                     (tuple[0]*q.tuple[1] - tuple[1]*q.tuple[0]));
                     //(1));	
    }
    
    float &operator[](int x) {
        return tuple[x];
    }

    // Other Special Operations
    point ToScreenCoordinates(int screenwidth, int screenheight)
    {        
        // Screen Space: (x, y) : ([0,width], [0,height])
        // clip space: (x,y):([-1:1],[-1:1])
        // so the -1.0f will move (0,0) to (-1, -1)
        return point((float)(tuple[DIM_X] / (float)(screenwidth/2.0f)) - 1.0f, 
                     (float)(tuple[DIM_Y] / (float)(screenheight/2.0f)) + 1.0f);   // top of the screen is y = 0
    }

    // Function is Inline due to it's standard nature
    void ToString()  {
        printf("point Class: { x: %f y: %f z: %f w: %f }\n", tuple[0], tuple[1], tuple[2]/*, tuple[3]*/);
    }

public:
    float tuple[POINT_DIM];	// tuple[3] == 1 always
};