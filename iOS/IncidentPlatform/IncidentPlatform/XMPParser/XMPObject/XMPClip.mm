//
//  XMPClip.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPClip.h"

@implementation XMPClip

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_CLIP;
    
    CRM([self ConstructClip], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructClip {
    RESULT r = R_SUCCESS;
    
    XMPValue trackNameVal = [self GetAttributeValueWithName:@"name"];
    if(trackNameVal.m_ValueType != XMP_VALUE_INVALID)
        m_strName = [[NSString alloc] initWithCString:trackNameVal.GetPszValue() encoding:NSUTF8StringEncoding];
    else
        m_strName = @"";
    
    if([self HasAttributeWithName:@"startbeat"])
        [self GetAttributeValueWithName:@"startbeat"].GetValueDouble(&m_startbeat);
    else
        m_startbeat = 0.0f;
    
    if([self HasAttributeWithName:@"endbeat"])
        [self GetAttributeValueWithName:@"endbeat"].GetValueDouble(&m_endbeat);
    else
        m_endbeat = 0.0f;
    
    if([self HasAttributeWithName:@"looping"])
        [self GetAttributeValueWithName:@"looping"].GetValueBool((bool*)(&m_fLoop));
    else
        m_fLoop = false;
    
    if([self HasAttributeWithName:@"loopstart"])
        [self GetAttributeValueWithName:@"loopstart"].GetValueDouble(&m_loopstart);
    else
        m_loopstart = 0.0f;
    
    if([self HasAttributeWithName:@"looplength"])
        [self GetAttributeValueWithName:@"looplength"].GetValueDouble(&m_looplength);
    else
        m_looplength = 0.0f;
    
    if([self HasAttributeWithName:@"clipstart"])
        [self GetAttributeValueWithName:@"clipstart"].GetValueDouble(&m_clipstart);
    else
        m_clipstart = 0.0f;
    
    if([self HasAttributeWithName:@"cliplength"])
        [self GetAttributeValueWithName:@"cliplength"].GetValueDouble(&m_cliplength);
    else
        m_cliplength = (m_endbeat - m_clipstart);
    
Error:
    return r;
}

@end
