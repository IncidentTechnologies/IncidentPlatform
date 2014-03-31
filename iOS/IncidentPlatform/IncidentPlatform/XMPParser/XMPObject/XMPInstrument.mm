//
//  XMPInstrument.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPInstrument.h"

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

@end
