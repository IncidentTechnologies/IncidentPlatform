//
//  XMPSampler.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@class XMPBank;

@interface XMPSampler : XMPObject {
    // name
    NSString *m_strName;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithName:(NSString*)name;
-(RESULT)ConstructSampler;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

-(RESULT)AddBank:(XMPBank*)bank;
-(XMPBank*)AddNewBankWithName:(NSString*)name;

@end
