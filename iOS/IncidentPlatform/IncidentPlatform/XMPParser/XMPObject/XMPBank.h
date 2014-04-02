//
//  XMPBank.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@class XMPSample;

@interface XMPBank : XMPObject {
    // name
    NSString *m_strName;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithName:(NSString*)name;
-(RESULT)ConstructBank;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

-(RESULT)AddSample:(XMPSample*)sample;
-(XMPSample*)AddNewSampleWithValue:(int)value path:(NSString*)path;

@end
