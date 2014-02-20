#import "XMPNote.h"

@implementation XMPNote

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_NOTE;
    
    CRM([self ConstructNote], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructNote {
    RESULT r = R_SUCCESS;
    
    m_duration = 0;
    m_beatstart = 0;
    
    m_MidiNote = [XMPNote MidiNoteNumberFromNote:[self GetAttributeValueWithName:@"value"].GetPszValue()];
    
    [self GetAttributeValueWithName:@"duration"].GetValueDouble(&m_duration);
    
    if([self HasAttributeWithName:@"measurestart"])
        [self GetAttributeValueWithName:@"measurestart"].GetValueDouble(&m_beatstart);
    else if([self HasAttributeWithName:@"beatstart"])
        [self GetAttributeValueWithName:@"beatstart"].GetValueDouble(&m_beatstart);
    
Error:
    return r;
}


+(int)MidiNoteNumberFromNote:(char *)pszName {
    int midiVal = 0;
    int octaveIndex = 1;
    
    midiVal = (toupper(pszName[0]) - (int)('A'));
    if(midiVal < 0 || midiVal > 6)
        return -1;  // err: only allowed values are [a-g|A-G]
    
    // Shift so that C is at 0
    midiVal -= 2;
    if(midiVal < 0) midiVal += 7;
    midiVal = midiVal * 2;
    if(midiVal > 4) // adjust for the E/F half step
        midiVal -= 1;
    
    if(strlen(pszName) == 3) {
        // has a # or b denomination
        if(pszName[1] == '#')
            midiVal += 1;
        else if(tolower(pszName[1]) == 'b')
            midiVal -= 1;
        else
            return -1;  // sharp flat designator not b or #
        
        octaveIndex = 2;
    }
    
    if(midiVal < 0 || midiVal > 11)
        return -1;     // Err: No Cb or B#s allowed!
    
    int octaveVal = (int)(pszName[octaveIndex] - (int)('0'));
    if(octaveVal < 0 || octaveVal > 9)
        return -1;  // Err: Octave val must be [0-9]
    
    return midiVal + (12 * (octaveVal + 1));
}

@end