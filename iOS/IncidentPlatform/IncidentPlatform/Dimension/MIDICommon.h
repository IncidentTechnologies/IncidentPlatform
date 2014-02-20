#pragma once

#include <string>

// This contains some common MIDI typedefs and enums

typedef enum MidiStatusMessage
{
    MIDI_NOTE_ON    = 0x90,     // d1: note number d2: velocity
    MIDI_NOTE_OFF   = 0x80,     // d1: note number d2: velocity
    MIDI_POLY_AFTER = 0xA0,     // d1: note number d2: Pressure (Polyphonic aftertouch)
    MIDI_CTRL       = 0xB0,     // d1: Controller Number d2: data (control change)
    MIDI_PRGM_CH    = 0xC0,     // d1: Program Number d2: unused (program change)
    MIDI_CHN_AFTER  = 0xD0,     // d1: Pressure d2: unused (channel aftertouch)
    MIDI_PITCH      = 0xE0      // d1: LSB d2: MSB (pitch control change)
} MIDI_STATUS_MESSAGE;

static MidiStatusMessage g_rgMidiStatusMessages[] = {MIDI_NOTE_ON,   
                                                     MIDI_NOTE_OFF,  
                                                     MIDI_POLY_AFTER,
                                                     MIDI_CTRL,      
                                                     MIDI_PRGM_CH,   
                                                     MIDI_CHN_AFTER, 
                                                     MIDI_PITCH};     

static int g_rgMidiStatusMessages_n = sizeof(g_rgMidiStatusMessages) / sizeof(g_rgMidiStatusMessages[0]);

static char* g_rgpszMidiStatusMessages[] = {"noteon",   
                                            "noteoff",  
                                            "polyafter",
                                            "midictrl",      
                                            "prgmch",   
                                            "chnafter", 
                                            "pitch"};     

static int g_rgpszMidiStatusMessages_n = sizeof(g_rgpszMidiStatusMessages) / sizeof(g_rgpszMidiStatusMessages[0]);

MidiStatusMessage ConvertStringMidiMsg(char *pszMidiMsg);