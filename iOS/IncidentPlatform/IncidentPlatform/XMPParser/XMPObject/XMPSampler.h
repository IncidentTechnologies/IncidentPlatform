//
//  XMPSampler.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPSampler : XMPObject {
    // name
    NSString *m_strName;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(RESULT)ConstructSampler;

@end
