//
//  XMPGtarNote.h
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPGtarNote : XMPObject {
    int m_string;
    int m_fret;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(int) StringNumberFromNSString:(NSString*)nssString;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
