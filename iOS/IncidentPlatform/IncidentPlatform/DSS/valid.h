#pragma once

#include "RESULT.h"
#include "EHM.h"

// The valid adapter class is to allow any object to define itself
// as either valid or not.  This can be utilized by a garbage collector
// or other smart organization algorithm to check for validity before moving 
// forward with anything else

// More importantly invalidate could be used to indicate that a constructor has failed
// and that the resulting object is garbage

// EHM Macro for valid
// Check Valid Result (Message)
#define CVR(o) if(!CheckValid(o)){ DEBUG_MSG(CurrentFileLine); r = R_INVALID; DEBUG_MSG("Error: 0x%x\n", r); goto Error; }
#define CVRM(o, msg, ...) if(!CheckValid(o)){ DEBUG_MSG(CurrentFileLine); DEBUG_MSG(msg, __VA_ARGS__); DEBUG_MSG("Error: 0x%x\n", r); r = R_INVALID; goto Error; }

namespace dss {

class valid
{
public:
    valid(bool valid = false);
    ~valid();

    bool IsValid();
    int GetValid();
    
    RESULT Validate();
    RESULT Invalidate();

private:
    int m_fValid;   // use an integer to help with error detection (corruption detection)
};
    
} // namespace dss

// Valid Checking EHM based routines
bool CheckValid(void *obj);