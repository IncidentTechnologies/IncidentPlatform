// color is the same as a point
// however it is specifically used for color

/* LOOK: DirectX is stupid and passes the color of a vertex along with the vertex itself
         for now as long as a 4 tuple of chars is asserted first and kept in sync with 
         the float tuple then there are no problems but this may be an issue when 
         directX asks for more information in the vertex format */

// Colors are DWORD
#pragma once

//#include <windows.h>

#define COLOR_DIMENSIONS 4

#define COLOR_A 0
#define COLOR_R 1
#define COLOR_G 2
#define COLOR_B 3

#define cfA(c) c.m_fTuple[COLOR_A]
#define cfR(c) c.m_fTuple[COLOR_R]
#define cfG(c) c.m_fTuple[COLOR_G]
#define cfB(c) c.m_fTuple[COLOR_B]

#define cbA(c) (char)(c.m_fTuple[COLOR_A] * 255)
#define cbR(c) (char)(c.m_fTuple[COLOR_R] * 255)
#define cbG(c) (char)(c.m_fTuple[COLOR_G] * 255)
#define cbB(c) (char)(c.m_fTuple[COLOR_B] * 255)

#define RGB(r,g,b) (1.0f, (float)(r / 255), (float)(g / 255), (float)(b / 255))
#define ARGB(a,r,g,b) (a, r, g, b)

typedef enum ColorValue
{
    COLOR_RED,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_PINK,
    COLOR_BLACK,
    COLOR_WHITE,
    COLOR_INVALID
} COLOR_VALUE;

class color
{
public:
    color();
    color(COLOR_VALUE cv);
    color(float Tuple[COLOR_DIMENSIONS]);
    color(float A, float R, float G, float B);
    color(unsigned char A, unsigned char R, unsigned char G, unsigned char B);
    
    ~color()
    {
        /* empty stub */
    }

    unsigned long GetColor32()
    {
        unsigned long result = 0;
        for(int i = 0; i < COLOR_DIMENSIONS; i++)
        {
            result += ((unsigned char)((255) * m_fTuple[i])) << (8 * i);
        }

        return result;
    }

    // Assignment
    color & operator=(const color &rhs);
    // Compound Assignment
    color & operator+=(const color &rhs);
    color & operator-=(const color &rhs);
    color & operator*=(const float &rhs);
    // Binary Arithmetic
    const color operator+(const color &rhs);
    const color operator-(const color &rhs);
    const color operator*(const float &rhs);
    // ************************************************************************

public:
    // Our Color Tuple
    float m_fTuple[COLOR_DIMENSIONS];
};

/*
// Some convenient colors
#define COLOR_WHITE     color(1.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_BLACK     color(1.0f, 0.0f, 0.0f, 0.0f)
#define COLOR_R       color(1.0f, 1.0f, 0.0f, 0.0f)
#define COLOR_G     color(1.0f, 0.0f, 1.0f, 0.0f)
#define COLOR_B      color(1.0f, 0.0f, 0.0f, 1.0f)*/