//
//  XMPData.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/7/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

typedef enum {
    DATA_ENCODING_BASE64,
    DATA_ENCODING_INVALID
} DATA_ENCODING;

@interface XMPData : XMPObject {
    DATA_ENCODING m_encoding;
    unsigned long int m_datasize;
    unsigned long int m_length; 
    
    NSData *m_data;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(RESULT) ConstructData;
-(DATA_ENCODING) ConvertEncodingString:(char*)pszEncoding;

@end
