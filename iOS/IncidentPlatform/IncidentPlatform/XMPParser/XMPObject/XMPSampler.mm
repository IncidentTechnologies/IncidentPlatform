//
//  XMPSampler.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPSampler.h"

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

@end
