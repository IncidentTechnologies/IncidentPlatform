//
//  XMPConent.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPContent.h"

@implementation XMPContent

-(id) init {
    RESULT r = R_SUCCESS;
    
    CPRM((self = [super init]), "XMPContent: init: Failed to init super");
    m_type = XMP_OBJECT_CONTENT;
    m_Name = @"content";
    
    return self;
Error:
    return NULL;
}


-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_CONTENT;
    
    CRM([self ConstructContent], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructContent {
    RESULT r = R_SUCCESS;
    
    // Do nothing for now
    
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
