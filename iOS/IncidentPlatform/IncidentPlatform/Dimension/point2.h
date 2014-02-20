#pragma once

// The point2 class represents a point with only two dimensions
// Eventually it would be preferrable to add templates to the 
// main point class but at the moment it is not needed

#define POINT2_DIM 2

class point2
{
public:
    point2()
    {
        memset(tuple, 0, sizeof(tuple));
    }

    ~point2()
    {
        /*empty stub*/
    }

    point2(float x, float y) 
    {
        tuple[0]=x;
        tuple[1]=y;
    }

    /*point2(float Tuple[POINT2_DIM]) 
    {
        for(int i = 0; i < POINT2_DIM; i++)
            tuple[i] = Tuple[i];
    }*/

    void set(float x, float y)
    {
        tuple[0]=x;
        tuple[1]=y;
    }

    // Arithmetic Operators
    point2 operator+ (point2 q) 
    {
        return point2(tuple[0] + q.tuple[0], tuple[1] + q.tuple[1]);         
    }

    const point2 &operator+=(const point2 &q)
    {
        tuple[0] += q.tuple[0];
        tuple[1] += q.tuple[1];
        return *this;
    }

    point2 operator- (point2 q) 
    {
        return point2(tuple[0] - q.tuple[0], tuple[1] - q.tuple[1]);
    }

    // Scalar Multiplication
    point2 operator* (float c) 
    {
        return point2(tuple[0]*c, tuple[1]*c);
    }

    bool operator== (point2 q)
    {
        return( (tuple[0] == q.tuple[0]) && 
                (tuple[1] == q.tuple[1]));
    }

    bool operator!= (point2 q)
    {
        return( (tuple[0] != q.tuple[0]) || 
                (tuple[1] != q.tuple[1]));
    }

    friend point2 operator* (float c, point2 q) 
    {
        return point2(c*q.tuple[0], c*q.tuple[1]); 
    };
    
    float &operator[](int x) {
        return tuple[x];
    }

public:
    float tuple[POINT2_DIM];
};