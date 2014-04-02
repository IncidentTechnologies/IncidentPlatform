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
-(id) initWithValue:(int)value Duration:(double)duration Beatstart:(double)beat;
-(RESULT)ConstructNote;

+(int)MidiNoteNumberFromNote:(char *)pszName;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
