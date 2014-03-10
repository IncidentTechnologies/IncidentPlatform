//
//  XMPData.m
//  IncidentPlatform
//
//  Created by Idan Beck on 3/7/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPData.h"

@implementation XMPData

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_DATA;
    
    [self ConstructData];
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructData {
    RESULT r = R_SUCCESS;
    
    XMPValue datasize = [self GetAttributeValueWithName:@"datasize"];
    if(datasize.m_ValueType == XMP_VALUE_INTEGER)
        datasize.GetValueInt((long *)(&m_datasize));
    
    // Get the encoding
    XMPValue encoding = [self GetAttributeValueWithName:@"dataencoding"];
    if(encoding.m_ValueType != XMP_VALUE_INVALID)
        m_encoding = [self ConvertEncodingString:encoding.GetPszValue()];
    else
        m_encoding = DATA_ENCODING_INVALID;
    
    XMPValue dataLength = [self GetAttributeValueWithName:@"length"];
    if(dataLength.m_ValueType == XMP_VALUE_INTEGER)
        dataLength.GetValueInt((long*)(&m_length));
    
    m_data = [[NSData alloc] initWithBase64EncodedString:[self Text] options:0];
    int a = 5;
    
Error:
    return r;
}

-(DATA_ENCODING) ConvertEncodingString:(char*)pszEncoding {
    if(strcmp(pszEncoding, "base64") == 0)
        return DATA_ENCODING_BASE64;
    
    return DATA_ENCODING_INVALID;
}

@end
