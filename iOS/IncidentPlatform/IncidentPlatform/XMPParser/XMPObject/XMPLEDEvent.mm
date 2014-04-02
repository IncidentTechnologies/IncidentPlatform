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

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    // Shouldn't have any children, but if it does
    /*
     for(XMPObject *child in m_contents) {
     XMPNode *childNode = [child CreateXMPNodeFromObjectWithParent:node];
     node->AddChild(childNode);
     }
     */
    
    return node;
}

@end
