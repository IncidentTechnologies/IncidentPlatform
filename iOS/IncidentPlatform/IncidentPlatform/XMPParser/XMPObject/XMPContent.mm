//
//  XMPConent.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPContent.h"

@implementation XMPContent

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

@end
