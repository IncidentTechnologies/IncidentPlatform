//
//  XMPUndock.m
//  gtarLearn
//
//  Created by Kate Schnippering on 6/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPUndock.h"

@implementation XMPUndock

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_UNDOCK;
    
    //CRM([self ConstructUndock], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructUndock {
    RESULT r = R_SUCCESS;
    
Error:
    return r;
}


-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    // Shouldn't have any children, but if it does
    for(XMPObject *child in m_contents) {
        if(child->m_type != XMP_OBJECT_OBJECT) {
            XMPNode *childNode = [child CreateXMPNodeFromObjectWithParent:node];
            node->AddChild(childNode);
        }
    }
    
    return node;
}

@end
