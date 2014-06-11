//
//  XMPText.m
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPLoop.h"

@implementation XMPLoop

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_LOOP;
    
    // Set up the XMP Loop
    CRM([self ConstructLoop], "initWithLoopName: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(id)initWithLoopName:(NSString *)LoopName andSuccess:(NSString *)Success andFail:(NSString *)Fail
{
    RESULT r = R_SUCCESS;
    
    m_LoopName = [[NSString alloc] initWithString:LoopName];
    m_Success = [[NSString alloc] initWithString:Success];
    m_Fail = [[NSString alloc] initWithString:Fail];
    
    m_repeat = 0;
    m_complete = 0.5;
    
    m_xmpNode = NULL;
    
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_LOOP;
    m_Name = @"loop";
    
    return self;
Error:
    return NULL;
    
}

-(RESULT)ConstructLoop {
    RESULT r = R_SUCCESS;
    
    m_LoopName = NULL;
    m_Success = NULL;
    m_Fail = NULL;
    m_repeat = 0;
    m_complete = 0;
    
    NSString * tempName;
    
    XMPValue loopNameVal = [self GetAttributeValueWithName:@"name"];
    if(loopNameVal.m_ValueType != XMP_VALUE_INVALID){
        tempName = [[NSString alloc] initWithCString:loopNameVal.GetPszValue() encoding:NSUTF8StringEncoding];
    }else{
        tempName = @"";
    }
    
    m_LoopName = tempName;
    
    m_Success = [self GetChildTextWithName:@"success"];
    
    m_Fail = [self GetChildTextWithName:@"fail"];
    
    XMPValue loopRepeatVal = [self GetAttributeValueWithName:@"repeat"];
    if(loopRepeatVal.m_ValueType != XMP_VALUE_INVALID){
        loopRepeatVal.GetValueInt(&m_repeat);
    }
    
    XMPValue loopCompleteVal = [self GetAttributeValueWithName:@"complete"];
    if(loopCompleteVal.m_ValueType != XMP_VALUE_INVALID){
        loopCompleteVal.GetValueDouble(&m_complete);
    }
    
Error:
    return r;
}


-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    node->AddAttribute(new XMPAttribute((char *)"name", (char *)[m_LoopName UTF8String]));
    
    node->AddAttribute(new XMPAttribute((char *)"repeat", m_repeat));
    
    node->AddAttribute(new XMPAttribute((char *)"complete", m_complete));
    
    XMPNode *successNode = new XMPNode((char *)"success", node);
    node->AddChild(successNode);
    
    XMPNode *failNode = new XMPNode((char *)"fail", node);
    node->AddChild(failNode);
    
    // Shouldn't have any children, but if it does
    for(XMPObject *child in m_contents) {
        if(child->m_type != XMP_OBJECT_OBJECT) {
            XMPNode *childNode = [child CreateXMPNodeFromObjectWithParent:node];
            node->AddChild(childNode);
        }
    }
    
    return node;
}


@end
