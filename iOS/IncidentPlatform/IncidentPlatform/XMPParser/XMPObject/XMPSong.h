//
//  XMPSong.h
//  gtarLearn

#import <Foundation/Foundation.h>
#import "XMPObject.h"

@class XMPContent;

@class XMPTrack;

@interface XMPSong : XMPObject {
    NSString *m_Description;
    NSString *m_Title;
    NSString *m_Author;
    double m_tempo;
    
    XMPContent *m_XMPContent;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithSongTitle:(NSString*)title author:(NSString*)author description:(NSString*)description;

-(RESULT)ConstructSong;
-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

-(RESULT)AddTrack:(XMPTrack*)track;
-(XMPTrack*)AddNewTrackWithName:(NSString *)name;

@end