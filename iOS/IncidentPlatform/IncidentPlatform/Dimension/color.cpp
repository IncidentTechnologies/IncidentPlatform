#include "color.h"
#include "EHM.h"
#include <string>

using namespace dss;

color::color()
{
    for(int i = 0; i < COLOR_DIMENSIONS; i++)
    {
        m_fTuple[i] = 1.0f;        
    }   
}

color::color(COLOR_VALUE cv)
{
    RESULT r = R_SUCCESS;

    CBRM((cv <= COLOR_INVALID), "color: Invalid color chosen!");

    switch(cv)
    {
        case COLOR_RED:
        {
            m_fTuple[COLOR_A] = 1.0f;
            m_fTuple[COLOR_R] = 1.0f;
            m_fTuple[COLOR_G] = 0.0f;
            m_fTuple[COLOR_B] = 0.0f;
        } break;

        case COLOR_BLUE:
        {
            m_fTuple[COLOR_A] = 1.0f;
            m_fTuple[COLOR_R] = 0.0f;
            m_fTuple[COLOR_G] = 0.0f;
            m_fTuple[COLOR_B] = 1.0f;
        } break;

        case COLOR_GREEN:
        {
            m_fTuple[COLOR_A] = 1.0f;
            m_fTuple[COLOR_R] = 0.0f;
            m_fTuple[COLOR_G] = 1.0f;
            m_fTuple[COLOR_B] = 0.0f;
        } break;

        case COLOR_YELLOW:
        {
            m_fTuple[COLOR_A] = 1.0f;
            m_fTuple[COLOR_R] = 0.0f;
            m_fTuple[COLOR_G] = 1.0f;
            m_fTuple[COLOR_B] = 0.0f;
        } break;

        case COLOR_BLACK:
        {
            m_fTuple[COLOR_A] = 1.0f;
            m_fTuple[COLOR_R] = 0.0f;
            m_fTuple[COLOR_G] = 0.0f;
            m_fTuple[COLOR_B] = 0.0f;
        } break;

        case COLOR_WHITE:
        {
            m_fTuple[COLOR_A] = 1.0f;
            m_fTuple[COLOR_R] = 1.0f;
            m_fTuple[COLOR_G] = 1.0f;
            m_fTuple[COLOR_B] = 1.0f;
        } break; 

        default:
        {
            CBRM((0), "color: color %d not implemented!", (int)cv);
        } break;
    }

Error:
    memset(m_fTuple, 0, sizeof(m_fTuple));
}

color::color(float Tuple[COLOR_DIMENSIONS])
{
    for(int i = 0; i < COLOR_DIMENSIONS; i++)
    {
        if(Tuple[i] >= 0 && Tuple[i] <= 1) 
            m_fTuple[i] = Tuple[i];
        else if(Tuple[i] > 1) 
            m_fTuple[i] = 1;
        else if(Tuple[i] < 0) 
            m_fTuple[i] = 0;
    }
}

color::color(float A, float R, float G, float B)
{
    // Alpha
    if(A >= 0 && A <= 1) 
        m_fTuple[COLOR_A] = A;
    else if(A > 1) 
        m_fTuple[COLOR_A] = 1;
    else if(A < 0) 
        m_fTuple[COLOR_A] = 0;
    // RED
    if(R >= 0 && R <= 1) 
        m_fTuple[COLOR_R] = R;
    else if(R > 1) 
        m_fTuple[COLOR_R] = 1;
    else if(R < 0) 
        m_fTuple[COLOR_R] = 0;
    // GREEN
    if(G >= 0 && G <= 1) 
        m_fTuple[COLOR_G] = G;
    else if(G > 1) 
        m_fTuple[COLOR_G] = 1;
    else if(G < 0) 
        m_fTuple[COLOR_G] = 0;
    // BLUE
    if(B >= 0 && B <= 1) 
        m_fTuple[COLOR_B] = B;
    else if(B > 1) 
        m_fTuple[COLOR_B] = 1;
    else if(B < 0) 
        m_fTuple[COLOR_B] = 0;
}

color::color(unsigned char A, unsigned char R, unsigned char G, unsigned char B)
{

    m_fTuple[COLOR_A] = ((float)A / 255.0f);
    m_fTuple[COLOR_R] = ((float)R / 255.0f);
    m_fTuple[COLOR_G] = ((float)G / 255.0f);
    m_fTuple[COLOR_B] = ((float)B / 255.0f);

}

// Operator Overloading
// ************************************************************************

// Assignment
color & color::operator=(const color &rhs)
{ 
    for(int i = 0; i < COLOR_DIMENSIONS; i++)
    {
        m_fTuple[i] = rhs.m_fTuple[i];
    }
    
    return *this; 
}

// Compound Assignment
color & color::operator+=(const color &rhs)
{
    float temp_it;

    for(int i = 0; i < COLOR_DIMENSIONS; i++)
    {
        temp_it = m_fTuple[i] + rhs.m_fTuple[i];
        if(temp_it > 1) 
            temp_it = 1;

        m_fTuple[i] = temp_it;
    }

    return *this;
}

color & color::operator-=(const color &rhs)
{
    float temp_it;

    for(int i = 0; i < COLOR_DIMENSIONS; i++)
    {
        temp_it = m_fTuple[i] - rhs.m_fTuple[i];
        if(temp_it < 0) 
            temp_it = 0;

        m_fTuple[i] = temp_it;
    }

    return *this;
}

color & color::operator*=(const float &rhs)
{
    float temp_it;
    
    if(rhs >= 0)
    {
        for(int i = 0; i < COLOR_DIMENSIONS; i++)
        {
            temp_it = m_fTuple[i] * rhs;
            if(temp_it > 1) 
                temp_it = 1;

            m_fTuple[i] = temp_it;
        }
    }
    else
    {
        memset(m_fTuple, 0, sizeof(m_fTuple));
    }

    return *this;
}

// Binary Arithmetic
const color color::operator+(const color &rhs)
{
    return color(m_fTuple[COLOR_A] + rhs.m_fTuple[COLOR_A],
                 m_fTuple[COLOR_R] + rhs.m_fTuple[COLOR_R],
                 m_fTuple[COLOR_G] + rhs.m_fTuple[COLOR_G],
                 m_fTuple[COLOR_B] + rhs.m_fTuple[COLOR_B]);
}

const color color::operator-(const color &rhs)
{
    return color(m_fTuple[COLOR_A] - rhs.m_fTuple[COLOR_A],
                 m_fTuple[COLOR_R] - rhs.m_fTuple[COLOR_R],
                 m_fTuple[COLOR_G] - rhs.m_fTuple[COLOR_G],
                 m_fTuple[COLOR_B] - rhs.m_fTuple[COLOR_B]);
}

const color color::operator*(const float &rhs)
{
    return color(m_fTuple[COLOR_A] * rhs,
                 m_fTuple[COLOR_R] * rhs,
                 m_fTuple[COLOR_G] * rhs,
                 m_fTuple[COLOR_B] * rhs);
}