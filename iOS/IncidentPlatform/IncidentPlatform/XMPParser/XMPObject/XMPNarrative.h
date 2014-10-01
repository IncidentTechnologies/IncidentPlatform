//
//  XMPNarrative.h
//  gtarLearn
//
//  Created by Kate Schnippering on 6/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPNarrative : XMPObject {
    
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
