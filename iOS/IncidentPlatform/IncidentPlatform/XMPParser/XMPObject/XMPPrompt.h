//
//  XMPPrompt.h
//  gtarLearn
//
//  Created by Kate Schnippering on 6/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPPrompt : XMPObject {
    
    NSString *m_displaytext;
    NSString *m_okbuttontext;

    bool m_sound;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;

-(RESULT)ConstructPrompt;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
