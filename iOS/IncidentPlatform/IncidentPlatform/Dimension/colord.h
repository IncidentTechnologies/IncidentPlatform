#pragma once

// Double word implementation of color
// this is because directx doesn't like the 
// real implementation although color is still existent
// until we can find a way to merge these two

#define COLORD_DIMENSIONS 4

#define COLORD_ALPHA 0
#define COLORD_RED 1
#define COLORD_GREEN 2
#define COLORD_BLUE 3

#include "color.h"


class colord
{
public:
    // Constructors
    colord();
    colord(float A, float R, float G, float B);
    colord(unsigned long c);
    colord(char A, char R, char G, char B);
    colord(int A, int R, int G, int B);
    colord(char Tuple[COLORD_DIMENSIONS]);
    colord(color c);
    
    // Destructor
    ~colord();

    // Set Operations
    void set(char A, char R, char G, char B);
    void set(float A, float R, float G, float B);

    // Gets
    unsigned char GetAlpha(){ return m_chTuple[COLORD_ALPHA]; }
    unsigned char GetRed(){ return m_chTuple[COLORD_RED]; }
    unsigned char GetGreen(){ return m_chTuple[COLORD_GREEN]; }
    unsigned char GetBlue(){ return m_chTuple[COLORD_BLUE]; }

    // Assignment
    colord & operator=(const colord &rhs);
    
    // Compound Assignment
    colord & operator+=(const colord &rhs);
    colord & operator-=(const colord &rhs);
    colord & operator*=(const char &rhs);
    
    // Binary Arithmetic
    const colord operator+(const colord &rhs);
    const colord operator-(const colord &rhs);
    const colord operator*(const char &rhs);
    // ************************************************************************

public:
    // Our Color Tuple
    unsigned char m_chTuple[COLORD_DIMENSIONS];
};