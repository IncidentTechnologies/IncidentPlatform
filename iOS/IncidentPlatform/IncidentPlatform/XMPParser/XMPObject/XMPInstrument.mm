//
//  XMPInstrument.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPInstrument.h"
#import "XMPSampler.h"


@implementation XMPInstrument

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_INSTRUMENT;
    
    CRM([self ConstructInstrument], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(id) initWithName:(NSString*)name {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = NULL;
    CPRM((self = [super init]), "initWithParentLesson: Failed to init super");
    
    m_type = XMP_OBJECT_INSTRUMENT;
    m_Name = @"instrument";
    
    m_strName = [[NSString alloc] initWithString:name];
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructInstrument {
    RESULT r = R_SUCCESS;
    
    XMPValue trackNameVal = [self GetAttributeValueWithName:@"name"];
    if(trackNameVal.m_ValueType != XMP_VALUE_INVALID)
        m_strName = [[NSString alloc] initWithCString:trackNameVal.GetPszValue() encoding:NSUTF8StringEncoding];
    else
        m_strName = @"";
    
Error:
    return r;
}

-(RESULT)AddSampler:(XMPSampler*)sampler {
    [self AddXMPObject:sampler];
    return R_SUCCEED;
}

-(XMPSampler*)AddNewSamplerWithName:(NSString*)name {
    XMPSampler *sampler = [[XMPSampler alloc] initWithName:name];
    [self AddSampler:sampler];
    return sampler;
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
