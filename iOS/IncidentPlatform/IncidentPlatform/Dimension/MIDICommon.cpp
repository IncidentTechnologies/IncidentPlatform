#include "MIDICommon.h"

MidiStatusMessage ConvertStringMidiMsg(char *pszMidiMsg)
{
    for(int i = 0; i < g_rgpszMidiStatusMessages_n; i++)
        if(strcmp(g_rgpszMidiStatusMessages[i], pszMidiMsg) == 0)    
            return g_rgMidiStatusMessages[i];
    return (MidiStatusMessage)(0x00);
}