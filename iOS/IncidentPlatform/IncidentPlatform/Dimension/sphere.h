// This class defines a 3d sphere in space
// essentially all you need for this is a 
// point of origin and a radius

#pragma once

#define DEFAULT_VERTICAL_DIVISIONS 10
#define DEFAULT_ANGULAR_DIVISIONS 10

#define PI 3.141592653589793

#include "DimObject.h"
#include "point.h"
#include "vector.h"
#include "vertex.h"



class sphere :
    public DimObject
{
public:
    sphere(point ptOrigin, float fRadius) :
      DimObject(ptOrigin, DIM_SPHERE),
      m_fRadius(fRadius),
      m_iVerticalDivisions(DEFAULT_VERTICAL_DIVISIONS),
      m_iAngularDivisions(DEFAULT_ANGULAR_DIVISIONS),
      m_LongSize(1.0f),
      m_LatSize(1.0f)
    {
        SetIndexedSphere();
    }
    
    sphere(point ptOrigin, float fRadius, int iVerticalDivisions, int iAngularDivisions) :
      DimObject(ptOrigin, DIM_SPHERE),
      m_fRadius(fRadius),
      m_iVerticalDivisions(iVerticalDivisions - 2),
      m_iAngularDivisions(iAngularDivisions),
      m_LongSize(1.0f),
      m_LatSize(1.0f)
    {
        SetIndexedSphere();
    }

    sphere(point ptOrigin, float fRadius, int iVerticalDivisions, int iAngularDivisions, float LongSize, float LatSize) :
      DimObject(ptOrigin, DIM_SPHERE),
      m_fRadius(fRadius),
      m_iVerticalDivisions(iVerticalDivisions),
      m_iAngularDivisions(iAngularDivisions),
      m_LongSize(LongSize),
      m_LatSize(LatSize)
    {
        SetIndexedSphere();
    }

    RESULT SetSphere();
    RESULT SetIndexedSphere();

    point GetOrigin() {return m_ptOrigin; }
    int GetVerticalDivisions(){ return m_iVerticalDivisions; }
    int GetAngularDivisions(){ return m_iAngularDivisions; }
    float GetRadius() {return m_fRadius; }
    float GetLongSize(){ return m_LongSize; }
    float GetLatSize(){ return m_LatSize; }

    const char *ToString() 
    {
        return "Sphere";
    }

    ~sphere()
    {
        if(m_verts != NULL) 
        {
            //delete m_verts;   
        }
    }

    vertex* GetVertexBuffer()
    {
        return m_verts;
    }
    int GetVertexCount()
    {
        return m_cVerts;
    }

protected:
    float m_fRadius;

    // Sphere coordinate count based on
    // longitudinal and latitudinal sections
    // 2 + #divisionsVertical * #divisionsAngularily
    vertex *m_verts;
    int *m_indices;
    int m_indices_n;

    int m_iVerticalDivisions;
    int m_iAngularDivisions;
    int m_cVerts;

    float m_LongSize;   // Sets [0.0 1.0] the longitude size
    float m_LatSize;    // Sets [0.0 1.0] the latitude size
};

