//
//  XMPConent.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPContent : XMPObject {
    // empty
}


-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(RESULT)ConstructContent;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
