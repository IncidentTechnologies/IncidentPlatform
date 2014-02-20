#pragma once

typedef enum CommonControlType
{
    COMMON_CONTROL_TEXT,
    COMMON_CONTROL_INVALID
} COMMON_CONTROL_TYPE;

// Common Controls Main Include File
// Common controls do not actually contain any geometric data but serve as 
// containers for their internal components.  They are convenient containers
// so that functionality can be accessed without dealing with the platform 
// specifics
// *********************************
#include "IComposite.h"
// *********************************
#include "TextControl.h"    // A general text / edit control