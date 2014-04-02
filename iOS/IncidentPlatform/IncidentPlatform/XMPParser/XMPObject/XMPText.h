//
//  XMPText.h
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPText : XMPObject {
    NSString *m_text;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
