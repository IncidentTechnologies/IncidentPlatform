//
//  XMPTrack.h
//  gtarLearn

#import <Foundation/Foundation.h>
#import "XMPObject.h"

@class XMPClip;
@class XMPInstrument;

@interface XMPTrack : XMPObject {
    NSString *m_TrackName;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithName:(NSString*)name;
-(RESULT)ConstructTrack;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

-(RESULT)AddClip:(XMPClip*)xmpclip;
-(XMPClip*)AddNewClipWithName:(NSString*)name Start:(double)startbeat End:(double)endbeat;

-(RESULT)AddInstrument:(XMPInstrument*)instrument;
-(XMPInstrument*)AddNewInstrumentWithName:(NSString*)name;

@end
