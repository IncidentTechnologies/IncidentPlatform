//
//  XMPClear.h
//  gtarLearn
//
//  Created by Kate Schnippering on 6/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPClear : XMPObject {
    
    NSString *m_pictureId;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;

-(RESULT)ConstructClear;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
