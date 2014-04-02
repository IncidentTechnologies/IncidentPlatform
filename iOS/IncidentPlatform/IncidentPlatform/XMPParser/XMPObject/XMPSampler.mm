//
//  XMPSampler.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPSampler.h"
#import "XMPBank.h"


@implementation XMPSampler

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_SAMPLER;
    
    CRM([self ConstructSampler], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(id) initWithName:(NSString*)name {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = NULL;
    CPRM((self = [super init]), "initWithParentLesson: Failed to init super");
    
    m_type = XMP_OBJECT_SAMPLER;
    m_Name = @"sampler";
    
    m_strName = [[NSString alloc] initWithString:name];
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructSampler {
    RESULT r = R_SUCCESS;
    
    XMPValue trackNameVal = [self GetAttributeValueWithName:@"name"];
    if(trackNameVal.m_ValueType != XMP_VALUE_INVALID)
        m_strName = [[NSString alloc] initWithCString:trackNameVal.GetPszValue() encoding:NSUTF8StringEncoding];
    else
        m_strName = @"";
    
Error:
    return r;
}

-(RESULT)AddBank:(XMPBank*)bank {
    return [self AddXMPObject:bank];
}

-(XMPBank*)AddNewBankWithName:(NSString*)name {
    XMPBank *bank = [[XMPBank alloc] initWithName:name];
    [self AddBank:bank];
    return bank;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    node->AddAttribute(new XMPAttribute("name", (char*)[m_strName UTF8String]));
    
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
