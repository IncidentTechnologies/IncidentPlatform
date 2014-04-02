//
//  XMPText.m
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPInput.h"
#import "dss_list.h"
#import "XMPObjectFactory.h"

@implementation XMPInput

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    XMPValue xmpValBlock;
    NSString *tempString = NULL;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_INPUT;
    
    // Set up blocking
    m_fBlock = true;
    xmpValBlock = xmpNode->GetAttribute((char*)"block")->GetXMPValue();
    
    if(xmpValBlock.m_ValueType != XMP_VALUE_INVALID) {
        tempString = [[NSString alloc] initWithCString:xmpValBlock.GetPszValue() encoding:NSUTF8StringEncoding];
        
        if([[tempString lowercaseString] isEqualToString:@"true"] || [[tempString lowercaseString] isEqualToString:@"t"])
            m_fBlock = true;
        else
            m_fBlock = false;
    }
    
    return self;
Error:
    return NULL;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    if(m_fBlock)
        node->AddAttribute(new XMPAttribute("block", "true"));
    else
        node->AddAttribute(new XMPAttribute("block", "false"));
    
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
