// Some memory management code 
// For now this utilizes a STL::LIST this should eventually
// be replaced with our own list implementation.

// DEPENDS: currently this is dependent on windows code somewhat
// this should eventually be moved to an output log rather than
// debug spew or rather simply console spew

#pragma once

#include <list>
#include "EHM.h"

namespace dss {

struct ALLOC_INFO {
    DWORD address;
    DWORD size;
    char file[64];
    DWORD line;
};

typedef std::list<ALLOC_INFO*> AllocList;

void AddTrack(DWORD addr, DWORD asize, const char *fname, DWORD lnum);
void RemoveTrack(DWORD addr);
RESULT DumpUnfreed();

}