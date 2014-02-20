//
//  XMPSong.h
//  gtarLearn

#import <Foundation/Foundation.h>
#import "XMPObject.h"

@interface XMPSong : XMPObject {
    NSString *m_Description;
    NSString *m_Title;
    NSString *m_Author;
    double m_tempo;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(RESULT)ConstructSong;

@end