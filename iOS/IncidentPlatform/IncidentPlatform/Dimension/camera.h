// Camera Class
// ******************************************
// NOTE: the camera does not contain any dimensional information 
// but only spatial.  This is why it does not derive IDimObject

#include "DimObject.h"

#pragma once

class camera :
    public DimObject
{

public:
    camera() :
        DimObject(DIM_CAMERA, NULL, false)
    {
        /*empty stub*/
    }
    
    camera(point ptOrigin, point ptLookAtPosition, vector vUpDirection) :
        DimObject(ptOrigin, DIM_CAMERA, NULL, false),
        m_ptLookAtPosition(ptLookAtPosition),
        m_vUpDirection(vUpDirection)
    {
        m_vStrafe = (m_ptLookAtPosition - m_ptOrigin)%(m_vUpDirection);
        m_vStrafe.normalize();
    }

    ~camera()
    {

    }

    // Strafe Movements
    void Move(float fX, float fY, float fZ)
    {
        m_ptOrigin += point(fX, fY, fZ);
        m_ptLookAtPosition += point(fX, fY, fZ);
    }

    void MoveForward(float fForward)
    {
        vector vForward = (m_ptLookAtPosition - m_ptOrigin);    
        vForward.normalize();
        m_ptOrigin += fForward*vForward;
        m_ptLookAtPosition += fForward*vForward;
    }

    void MoveSideways(float fSide)
    {
        m_ptOrigin += fSide*m_vStrafe;
        m_ptLookAtPosition += fSide*m_vStrafe;
    }

    void RotateX(float fThetaX)
    {
        vector vLA = (m_ptLookAtPosition - m_ptOrigin);
        //point pLA = point(0, 0, 0) + vLA;

        float fMag = vLA.magnitude();

        vLA.VRotate(m_vStrafe, fThetaX); //= RotateAboutVectorFromOrigin(vLA, m_vStrafe, fThetaX);

        vLA.normalize();
        m_ptLookAtPosition = m_ptOrigin + vLA;
    }

    void RotateY(float fThetaY)
    {
        vector vLA = (m_ptLookAtPosition - m_ptOrigin);
        float fMag = vLA.magnitude();

        vLA.tuple[DIM_X] = vLA.tuple[0]*cos(fThetaY) + vLA.tuple[2]*sin(fThetaY);
        vLA.tuple[DIM_Y] = vLA.tuple[1];
        vLA.tuple[DIM_Z] = -1*vLA.tuple[0]*sin(fThetaY) + vLA.tuple[2]*cos(fThetaY);

        m_vStrafe.VRotate(m_vUpDirection, fThetaY); // = RotateAboutVectorFromOrigin(m_vStrafe, m_vUpDirection, fThetaY);

        vLA.normalize();
        m_vStrafe.normalize();

        m_ptLookAtPosition = m_ptOrigin + vLA;
    }

    void RotateZ(float fThetaZ)
    {
        // Need to rotate the up direction
    }

    const char *ToString() 
    {
        return "Camera";
    }

    point GetOrigin(){ return m_ptOrigin; }
    RESULT SetOrigin(point ptOrigin){ m_ptOrigin = ptOrigin; return R_SUCCESS; }
    point GetLookAtPosition(){ return m_ptLookAtPosition; }
    RESULT SetLookAtPosition(point ptLookAtPosition){ m_ptLookAtPosition = ptLookAtPosition; return R_SUCCESS; }
    vector GetUpDirection(){ return m_vUpDirection; }

    // Camera does not contain any vertex info (for now)
    vertex* GetVertexBuffer() {
        return NULL;
    }
    
    int GetVertexCount()
    {
        return 0;
    }

protected:
    point m_ptLookAtPosition;
    vector m_vUpDirection;
    vector m_vStrafe;
};