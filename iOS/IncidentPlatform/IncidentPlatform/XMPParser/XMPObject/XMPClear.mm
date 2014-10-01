//
//  XMPClear.m
//  gtarLearn
//
//  Created by Kate Schnippering on 6/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPClear.h"

@implementation XMPClear

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_CLEAR;
    
    //CRM([self ConstructClear], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructClear {
    RESULT r = R_SUCCESS;
    
    XMPValue idVal = [self GetAttributeValueWithName:@"id"];
    if(idVal.m_ValueType != XMP_VALUE_INVALID)
        m_pictureId = [[NSString alloc] initWithCString:idVal.GetPszValue() encoding:NSUTF8StringEncoding];
    else
        m_pictureId = @"";
    
Error:
    return r;
}


-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    node->AddAttribute(new XMPAttribute((char *)"id", (char*)[m_pictureId UTF8String]));
    
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
