#include "colord.h"
#include <string.h>

colord::colord()
{
    *m_chTuple = 0xFFFFFFFF;
}

colord::colord(float A, float R, float G, float B)
{
    if(A <= 1.0f) 
        m_chTuple[COLORD_ALPHA] = (char)(A * 255.0f);
    else
        m_chTuple[COLORD_ALPHA] = 255;

    if(R <= 1.0f) 
        m_chTuple[COLORD_RED] = (char)(R * 255.0f);
    else
        m_chTuple[COLORD_RED] = 255;

    if(G <= 1.0f) 
        m_chTuple[COLORD_GREEN] = (char)(G * 255.0f);
    else
        m_chTuple[COLORD_GREEN] = 255;

    if(B <= 1.0f) 
        m_chTuple[COLORD_BLUE] = (char)(B * 255.0f);
    else
        m_chTuple[COLORD_BLUE] = 255;
}

colord::colord(unsigned long c)
{
    *m_chTuple = c;
}

void colord::set(float A, float R, float G, float B)
{
    if(A <= 1.0f) 
        m_chTuple[COLORD_ALPHA] = (char)(A * 255.0f);
    else
        m_chTuple[COLORD_ALPHA] = 255;

    if(R <= 1.0f) 
        m_chTuple[COLORD_RED] = (char)(R * 255.0f);
    else
        m_chTuple[COLORD_RED] = 255;

    if(G <= 1.0f) 
        m_chTuple[COLORD_GREEN] = (char)(G * 255.0f);
    else
        m_chTuple[COLORD_GREEN] = 255;

    if(B <= 1.0f) 
        m_chTuple[COLORD_BLUE] = (char)(B * 255.0f);
    else
        m_chTuple[COLORD_BLUE] = 255;
}

void colord::set(char A, char R, char G, char B)
{
    m_chTuple[COLORD_ALPHA] = A;
    m_chTuple[COLORD_RED] = R;
    m_chTuple[COLORD_GREEN] = G;
    m_chTuple[COLORD_BLUE] = B;
}

colord::colord(char A, char R, char G, char B)
{
    m_chTuple[COLORD_ALPHA] = A;
    m_chTuple[COLORD_RED] = R;
    m_chTuple[COLORD_GREEN] = G;
    m_chTuple[COLORD_BLUE] = B;
}

colord::colord(int A, int R, int G, int B)
{
    if(A > 255) 
        m_chTuple[COLORD_ALPHA] = 255;
    else if(A < 0)
        m_chTuple[COLORD_ALPHA] = 0;
    else
        m_chTuple[COLORD_ALPHA] = A;

    if(R > 255) 
        m_chTuple[COLORD_RED] = 255;
    else if(R < 0)
        m_chTuple[COLORD_RED] = 0;
    else
        m_chTuple[COLORD_RED] = R;

    if(G > 255) 
        m_chTuple[COLORD_GREEN] = 255;
    else if(G < 0)
        m_chTuple[COLORD_GREEN] = 0;
    else
        m_chTuple[COLORD_GREEN] = G;

    if(B > 255) 
        m_chTuple[COLORD_BLUE] = 255;
    else if(B < 0)
        m_chTuple[COLORD_BLUE] = 0;
    else
        m_chTuple[COLORD_BLUE] = B;
}

colord::colord(char Tuple[COLORD_DIMENSIONS])
{
    for(int i = 0; i < COLORD_DIMENSIONS; i++)
        m_chTuple[i] = Tuple[i];
}

colord::colord(color c)
{
    *m_chTuple = c.GetColor32();
}

colord::~colord()
{
    /*empty stub*/
}

// Assignment
colord & colord::operator=(const colord &rhs)
{
    memcpy(m_chTuple, rhs.m_chTuple, sizeof(unsigned long));

    return *this;
}

// Compound Assignment
colord & colord::operator+=(const colord &rhs)
{
    int temp;
    
    for(int i = 0; i < COLORD_DIMENSIONS; i++)
    {
        temp = m_chTuple[i] + rhs.m_chTuple[i];
        if(temp > 255) 
            m_chTuple[i] = 255;
        else
            m_chTuple[i] = temp;
    }

    return *this;
}   

colord & colord::operator-=(const colord &rhs)
{
    int temp;

    for(int i = 0; i < COLORD_DIMENSIONS; i++)
    {
        temp = m_chTuple[i] - rhs.m_chTuple[i];
        if(temp < 0) 
            m_chTuple[i] = 0;
        else
            m_chTuple[i] = temp;
    }

    return *this;
}

colord & colord::operator*=(const char &rhs)
{
    int temp;

    for(int i = 0; i < COLORD_DIMENSIONS; i++)
    {
        temp = m_chTuple[i] * rhs;
        if(temp > 255) 
            m_chTuple[i] = 255;
        else
            m_chTuple[i] = temp;
    }

    return *this;
}

// Binary Arithmetic
const colord colord::operator+(const colord &rhs)
{
    return colord(m_chTuple[COLORD_ALPHA]   + rhs.m_chTuple[COLORD_ALPHA],
                  m_chTuple[COLORD_RED]     + rhs.m_chTuple[COLORD_RED],
                  m_chTuple[COLORD_GREEN]   + rhs.m_chTuple[COLORD_GREEN],
                  m_chTuple[COLORD_BLUE]    + rhs.m_chTuple[COLORD_BLUE]  );
}

const colord colord::operator-(const colord &rhs)
{
    return colord(m_chTuple[COLORD_ALPHA]   - rhs.m_chTuple[COLORD_ALPHA],
                  m_chTuple[COLORD_RED]     - rhs.m_chTuple[COLORD_RED],
                  m_chTuple[COLORD_GREEN]   - rhs.m_chTuple[COLORD_GREEN],
                  m_chTuple[COLORD_BLUE]    - rhs.m_chTuple[COLORD_BLUE]  );
}

const colord colord::operator*(const char &rhs)
{
    return colord(m_chTuple[COLORD_ALPHA]   * rhs,
                  m_chTuple[COLORD_RED]     * rhs,
                  m_chTuple[COLORD_GREEN]   * rhs,
                  m_chTuple[COLORD_BLUE]    * rhs );
}
// ************************************************************************