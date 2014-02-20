//
//  XMPText.m
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPLEDEvent.h"

@implementation XMPLEDEvent

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_LEDEVENT;
    
    // Set up the LED event
    
    return self;
Error:
    return NULL;
}

@end
