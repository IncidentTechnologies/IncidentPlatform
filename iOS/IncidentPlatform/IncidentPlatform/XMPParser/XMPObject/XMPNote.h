//
//  XMPNote.h
//  gtarLearn

#import <Foundation/Foundation.h>
#import "XMPObject.h"

@interface XMPNote : XMPObject {
    int m_MidiNote;
    double m_duration;
    
    double m_beatstart;     // beatstart will refer to parent as reference
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(RESULT)ConstructNote;

+(int)MidiNoteNumberFromNote:(char *)pszName;

@end
