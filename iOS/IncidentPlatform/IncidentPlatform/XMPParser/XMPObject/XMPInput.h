//
//  XMPText.h
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPInput : XMPObject {
    //NSString *m_text;
    bool m_fBlock;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;

@end
