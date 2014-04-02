//
//  XMPClip.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@class XMPNote;

@interface XMPClip : XMPObject {
    // name
    NSString *m_strName;
    
    // track time
    double m_startbeat;
    double m_endbeat;
    
    // loop
    BOOL m_fLoop;
    double m_loopstart;
    double m_looplength;
    
    // clip time
    double m_clipstart;
    double m_cliplength;

}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithName:(NSString*)name Start:(double)startbeat End:(double)endbeat;

-(RESULT)ConstructClip;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

-(RESULT)AddNote:(XMPNote*)note;
-(XMPNote*)AddNewNote:(int)value beatstart:(double)beat duration:(double)duration;

@end
