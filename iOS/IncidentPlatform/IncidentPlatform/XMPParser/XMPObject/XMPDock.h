//
//  XMPDock.h
//  gtarLearn
//
//  Created by Kate Schnippering on 6/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPDock : XMPObject {
    
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;

-(RESULT)ConstructDock;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
