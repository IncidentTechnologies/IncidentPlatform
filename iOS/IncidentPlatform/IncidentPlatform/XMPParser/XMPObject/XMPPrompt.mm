//
//  XMPPrompt.m
//  gtarLearn
//
//  Created by Kate Schnippering on 6/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPPrompt.h"

@implementation XMPPrompt

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_PROMPT;
    
    //CRM([self ConstructPrompt], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructPrompt {
    RESULT r = R_SUCCESS;
    
    XMPValue textVal = [self GetAttributeValueWithName:@"displaytext"];
    if(textVal.m_ValueType != XMP_VALUE_INVALID)
        m_displaytext = [[NSString alloc] initWithCString:textVal.GetPszValue() encoding:NSUTF8StringEncoding];
    else
        m_displaytext = @"";
    
    XMPValue okTextVal = [self GetAttributeValueWithName:@"okbuttontext"];
    if(okTextVal.m_ValueType != XMP_VALUE_INVALID)
        m_okbuttontext = [[NSString alloc] initWithCString:okTextVal.GetPszValue() encoding:NSUTF8StringEncoding];
    else
        m_okbuttontext = @"";
    
    if([self HasAttributeWithName:@"sound"])
        [self GetAttributeValueWithName:@"sound"].GetValueBool((bool*)(&m_sound));
    else
        m_sound = true;
    
    
Error:
    return r;
}


-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    node->AddAttribute(new XMPAttribute((char *)"displaytext", (char*)[m_displaytext UTF8String]));
    
    node->AddAttribute(new XMPAttribute((char *)"okbuttontext", (char*)[m_okbuttontext UTF8String]));
    
    node->AddAttribute(new XMPAttribute((char *)"sound", m_sound));
    
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
