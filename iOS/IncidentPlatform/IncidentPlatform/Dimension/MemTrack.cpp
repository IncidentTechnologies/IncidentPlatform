#include "MemTrack.h"
#include <stdio.h>
#include <stdlib.h>

using namespace dss;

AllocList *g_AllocList;

void AddTrack(DWORD addr, DWORD asize, const char *fname, DWORD lnum) {
    ALLOC_INFO *info;
    if(!g_AllocList)
        g_AllocList = new AllocList;

    info = new(ALLOC_INFO);
    info->address = addr;
    strncpy(info->file, fname, 63);
    info->line = lnum;
    info->size = asize;
    g_AllocList->insert(g_AllocList->begin(), info);
}

void RemoveTrack(DWORD addr) {
    AllocList::iterator i;
    if(!g_AllocList)
        return;
    
    for(i = g_AllocList->begin(); i != g_AllocList->end(); i++) {
        if((*i)->address == addr) {
            g_AllocList->remove((*i));
            break;
        }
    }
}

RESULT DumpUnfreed() {
    AllocList::iterator i;
    DWORD totalSize = 0;
    char buf[1024];
    
    if(!g_AllocList)
        return R_ERROR;

    for(i = g_AllocList->begin(); i != g_AllocList->end(); i++) {
        sprintf(buf, "%s:\tLINE %d,\t\tADDRESS %d\t%d unfreed\n",
                     (*i)->file, (*i)->line, (*i)->address, (*i)->size);
        printf(buf);
        totalSize += (*i)->size;
    }
    
    sprintf(buf, "-------------------------------------------------\n");
    printf(buf);
    sprintf(buf, "Total Unfreed: %d Bytes\n", totalSize);
    printf(buf);

    return (totalSize > 0) ? R_MEM_LEAKED : R_MEM_FREED;
}