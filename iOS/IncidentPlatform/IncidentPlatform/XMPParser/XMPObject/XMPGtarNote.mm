//
//  XMPGtarNote.m
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPGtarNote.h"

@implementation XMPGtarNote

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    XMPValue xmpValString;
    XMPValue xmpValFret;
    NSString *tempString = NULL;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_GTARNOTE;
    
    xmpValString = xmpNode->GetAttribute((char*)"string")->GetXMPValue();
    tempString = [[NSString alloc] initWithCString:xmpValString.GetPszValue() encoding:NSUTF8StringEncoding];
    m_string = [self StringNumberFromNSString:tempString];
    
    xmpValFret = xmpNode->GetAttribute((char*)"fret")->GetXMPValue();
    xmpValFret.GetValueInt((long int *)(&m_fret));
    
    return self;
Error:
    return NULL;
}

-(int) StringNumberFromNSString:(NSString*)nssString {
    int stringNum = -1;
    
    if([[nssString lowercaseString] isEqualToString:@"hie"] || [nssString intValue] == 6)
        stringNum = 6;
    else if([[nssString lowercaseString] isEqualToString:@"b"] || [nssString intValue] == 6)
        stringNum = 5;
    else if([[nssString lowercaseString] isEqualToString:@"g"] || [nssString intValue] == 6)
        stringNum = 4;
    else if([[nssString lowercaseString] isEqualToString:@"d"] || [nssString intValue] == 6)
        stringNum = 3;
    else if([[nssString lowercaseString] isEqualToString:@"a"] || [nssString intValue] == 6)
        stringNum = 2;
    else if([[nssString lowercaseString] isEqualToString:@"e"] || [nssString intValue] == 6)
        stringNum = 6;
    
    return stringNum;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    node->AddAttribute(new XMPAttribute("string", m_string));
    node->AddAttribute(new XMPAttribute("fret", m_fret));
    
    // Shouldn't have any children, but if it does
    /*
     for(XMPObject *child in m_contents) {
     XMPNode *childNode = [child CreateXMPNodeFromObjectWithParent:node];
     node->AddChild(childNode);
     }
     */
    
    return node;
}

@end
