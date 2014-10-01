//
//  XMPChord.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 7/28/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@class XMPNote;

@interface XMPChord : XMPObject
{
    NSString * m_name;
    
    double m_beatstart;
    double m_duration;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithName:(NSString*)name beatstart:(double)beatstart duration:(double)duration;

-(RESULT)ConstructChord;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

-(RESULT)AddNote:(XMPNote*)note;
-(XMPNote*)AddNewNote:(int)value beatstart:(double)beat duration:(double)duration;


@end
