//
//  XMPTrack.h
//  gtarLearn

#import <Foundation/Foundation.h>
#import "XMPObject.h"

@interface XMPTrack : XMPObject {
    NSString *m_TrackName;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(RESULT)ConstructTrack;

@end
