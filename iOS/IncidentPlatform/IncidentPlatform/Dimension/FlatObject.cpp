#include "FlatObject.h"


FlatObject::FlatObject( FLAT_OBJECT_TYPE fot ) :
    material(),
    m_FlatObjectType(fot),
    m_ptOrigin(),
    m_RotationX(0.0f),
    m_RotationY(0.0f),
    m_RotationZ(0.0f),
    m_Scale(1.0f),
    m_AxisAligned_f(false),
    m_Screen_f(false)
{
    m_id = IncrementFlatObjCount();
}

FlatObject::FlatObject( FLAT_OBJECT_TYPE fot, int ScreenWidth, int ScreenHeight ) :
    material(),
    m_FlatObjectType(fot),
    m_ptOrigin(),
    m_RotationX(0.0f),
    m_RotationY(0.0f),
    m_RotationZ(0.0f),
    m_Scale(1.0f),
    m_AxisAligned_f(false),
    //m_Screen_f(true),
    m_ScreenWidth(ScreenWidth),
    m_ScreenHeight(ScreenHeight)
{
    if(ScreenWidth == 0 || ScreenHeight == 0)
        m_Screen_f = false;
    else
        m_Screen_f = true;

    m_id = IncrementFlatObjCount();
}

FlatObject::FlatObject( point ptOrigin, FLAT_OBJECT_TYPE fot ) :
    material(),
    m_FlatObjectType(fot),
    m_ptOrigin(ptOrigin),
    m_RotationX(0.0f),
    m_RotationY(0.0f),
    m_RotationZ(0.0f),
    m_Scale(1.0f),
    m_AxisAligned_f(false),
    m_Screen_f(false)
{
    m_id = IncrementFlatObjCount();
}

FlatObject::FlatObject( point ptOrigin, FLAT_OBJECT_TYPE fot, int ScreenWidth, int ScreenHeight ) :
    material(),
    m_FlatObjectType(fot),
    m_ptOrigin(ptOrigin),
    m_RotationX(0.0f),
    m_RotationY(0.0f),
    m_RotationZ(0.0f),
    m_Scale(1.0f),
    m_AxisAligned_f(false),
    m_Screen_f(true),
    m_ScreenWidth(ScreenWidth),
    m_ScreenHeight(ScreenHeight)
{
    m_id = IncrementFlatObjCount();
}

FlatObject::FlatObject( const FlatObject &fo ) :
    material(),
    m_FlatObjectType(fo.m_FlatObjectType),
    m_ptOrigin(fo.m_ptOrigin),
    m_RotationX(fo.m_RotationX),
    m_RotationY(fo.m_RotationY),
    m_RotationZ(fo.m_RotationZ),
    m_Scale(fo.m_Scale),
    m_AxisAligned_f(fo.m_AxisAligned_f),
    m_Screen_f(fo.m_Screen_f),
    m_ScreenWidth(fo.m_ScreenWidth),
    m_ScreenHeight(fo.m_ScreenHeight)
{
    m_id = IncrementFlatObjCount();
}

void FlatObject::RotateX( float fx )
{
    if(!m_AxisAligned_f)
        m_RotationX += fx;
}

void FlatObject::RotateY( float fy )
{
    if(!m_AxisAligned_f)
        m_RotationY += fy;
}

void FlatObject::RotateZ( float fz )
{
    if(!m_AxisAligned_f)
        m_RotationZ += fz;
}

void FlatObject::SetRotateX( float fx )
{
    if(!m_AxisAligned_f)
        m_RotationX = fx;
}

void FlatObject::SetRotateY( float fy )
{
    if(!m_AxisAligned_f)
        m_RotationY = fy;
}

void FlatObject::SetRotateZ( float fz )
{
    if(!m_AxisAligned_f)
        m_RotationZ = fz;
}

void FlatObject::SetPosition( point pt )
{
    m_ptOrigin = pt;
}

void FlatObject::SetX( float x )
{
    if(m_Screen_f)
    {
        x = x / (float)m_ScreenWidth;
    }
    m_ptOrigin.tuple[DIM_X] = x;
}

void FlatObject::SetY( float y )
{
    if(m_Screen_f)
    {
        y = y / (float)m_ScreenHeight;
    }
    m_ptOrigin.tuple[DIM_Y] = y;
}

void FlatObject::SetZ( float z )
{
    // has no effect in blits except for depth ordering
    m_ptOrigin.tuple[DIM_Z] = z;
}

void FlatObject::MoveX( float x )
{
    if(m_Screen_f)
    {
        x = x / (float)m_ScreenWidth;
    }
    m_ptOrigin.tuple[DIM_X] += x;
}

void FlatObject::MoveY( float y )
{
    if(m_Screen_f)
    {
        y = y / (float)m_ScreenHeight;
    }
    m_ptOrigin.tuple[DIM_Y] += y;
}

void FlatObject::MoveZ( float z )
{
    // Has no effect in blits other than depth ordering
    m_ptOrigin.tuple[DIM_Z] += z;
}

RESULT FlatObject::ConvertToScreen( int ScreenWidth, int ScreenHeight )
{
    m_Screen_f = true;
    m_ScreenHeight = -ScreenHeight;
    m_ScreenWidth = ScreenWidth;

    return ConvertToScreen();
}

RESULT FlatObject::SetAttribute( int Attribute, void *pContext )
{
    RESULT r = R_NOT_IMPLEMENTED;
    // should be implemented by the appropriate object
Error:
    return r;
}

RESULT FlatObject::GetAttribute( int Attribute, void * &r_pContext )
{
    RESULT r = R_NOT_IMPLEMENTED;
    // should be implemented by the appropriate object
Error:
    return r;
}

void FlatObject::SetAxisAligned( bool AxisAligned )
{
    if(AxisAligned)
    {
        m_RotationX = 0.0f;
        m_RotationY = 0.0f;
        m_RotationZ = 0.0f;
    }
    m_AxisAligned_f = AxisAligned;
}