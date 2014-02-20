#include "nuke.h"

// Nuke functions
float MinDistancePointPoint(point a, point b)
{
    return sqrt((pX(b) - pX(a))*(pX(b) - pX(a)) +
                (pY(b) - pX(a))*(pY(b) - pX(a)) +
                (pZ(b) - pX(a))*(pZ(b) - pX(a)) );
}

// This is the minimum distance from a line
// where a2 - a1 define the line in space
float MinDistancePointLine(point a1, point a2, point P) {
    return ((a2 - a1)%(a1 - P)).magnitude() / (a2 - a1).magnitude();
}

float DistancePointPlane(point a1, point a2, point a3, point P)
{
    vector n = (a2 - a1) % (a3 - a1);
    n.normalize();

    // a1 use arbitrary, any point can be used
    return n*(P - a1);  
}

float DistancePointPlane(point p, plane pl)
{
    vector n = pl.GetNormal();
    n.normalize();

    return n*(p - pl.m_ptOrigin);
}

float DistancePointPlane(point p, plane *pPl)
{
    vector n = pPl->GetNormal();
    n.normalize();

    return n*(p - pPl->m_ptOrigin);
}

// This will return the scalar multiplier required to intersect
// a ray with a plane
float VectorPlaneIntersect(point a1, point a2, point a3, point P, vector v)
{
    vector n = (a2 - a1)%(a3 - a1);
    n.normalize();
    // now plane is defined by normal and a1
    if(n*v == 0) 
    {
        // vector and plane are parallel
        //return numeric_limits<float>::infinity();
        return 0;
    }
    else
    {
        return (n*(a1 - P))/(n*v);
    }
}

// This is similar to above but uses instead two points to define
// the ray
float VectorPlaneIntersect(point a1, point a2, point a3, point P2, point P1)
{
    vector n = (a2 - a1)%(a3 - a1);
    n.normalize();
    // now plane is defined by normal and a1
    if(n*(P2-P1) == 0) 
    {
        // vector and plane are parallel
        //return numeric_limits<float>::infinity();
        return 0;
    }
    else
    {
        return (n*(a1 - P1))/(n*(P2 - P1));
    }
}

// Returns a value, if value is between 0 and 1 then closest
// point is not on the line
//
// (x3 - x1)(x2 - x1) + (y3 - y1)(y2 - y1) + (z3 - z1)(z2 - z1)
// -----------------------------------------------------------
// (x2 - x1)(x2 - x1) + (y2 - y1)(y2 - y1) + (z2 - z1)(z2 - z1)

float LineSphereIntersect(point a1, point a2, point P, float radius)
{
    float a = pow((pX(a2) - pX(a1)), 2) + 
              pow((pY(a2) - pY(a1)), 2) +
              pow((pZ(a2) - pZ(a1)), 2) ;
    
    float b = 
        2*(
            (pX(a2) - pX(a1)*(pX(a1) - pX(P))) +   
            (pY(a2) - pY(a1)*(pY(a1) - pY(P))) +
            (pZ(a2) - pZ(a1)*(pZ(a1) - pZ(P)))
          );
    
    float c = pow(pX(P), 2) + pow(pY(P), 2) + pow(pZ(P), 2) +
              pow(pX(a1), 2) + pow(pY(a1), 2) + pow(pZ(a1), 2) -
              2*(pX(P)*pX(a1) + pY(P)*pY(a1) + pZ(P)*pZ(a1)) -
              pow(radius, 2);
    
    return b*b - 4 * a *c;

    /*return ( (pX(P) - pX(a1))*(pX(a2) - pX(a1)) + 
             (pY(P) - pY(a1))*(pY(a2) - pY(a1)) +
             (pZ(P) - pZ(a1))*(pZ(a2) - pZ(a1)) ) /
           ( (pX(a2) - pX(a1))*(pX(a2) - pX(a1)) +
             (pY(a2) - pY(a1))*(pY(a2) - pY(a1)) +
             (pZ(a2) - pZ(a1))*(pZ(a2) - pZ(a1)) );*/
}