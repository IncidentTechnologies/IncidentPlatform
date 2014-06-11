//
//  XMPText.h
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPLoop : XMPObject {
    
    NSString * m_LoopName;
    NSString * m_Success;
    NSString * m_Fail;
    
    long m_repeat;
    double m_complete;
    
}

- (id) initWithLoopName:(NSString *)LoopName andSuccess:(NSString *)Success andFail:(NSString *)Fail;

- (id) initWithXMPNode:(XMPNode*)xmpNode;
- (RESULT)ConstructLoop;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
