//
//  XMPDelay.h
//  gtarLearn
//
//  Created by Kate Schnippering on 6/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPDelay : XMPObject {
    
    double m_duration;
    
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;

-(RESULT)ConstructDelay;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
