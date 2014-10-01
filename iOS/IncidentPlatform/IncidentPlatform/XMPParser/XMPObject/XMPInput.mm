//
//  XMPText.m
//  gtarLearn
//
//  Created by Idan Beck on 12/18/13.
//  Copyright (c) 2013 Incident Technologies, Inc. All rights reserved.
//

#import "XMPInput.h"
#import "dss_list.h"
#import "XMPObjectFactory.h"

@implementation XMPInput

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    XMPValue xmpValBlock;
    NSString *tempString = NULL;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_INPUT;
    
    // Set up blocking
    m_fBlock = true;
    xmpValBlock = xmpNode->GetAttribute((char*)"block")->GetXMPValue();
    
    if(xmpValBlock.m_ValueType != XMP_VALUE_INVALID) {
        tempString = [[NSString alloc] initWithCString:xmpValBlock.GetPszValue() encoding:NSUTF8StringEncoding];
        
        if([[tempString lowercaseString] isEqualToString:@"true"] || [[tempString lowercaseString] isEqualToString:@"t"])
            m_fBlock = true;
        else
            m_fBlock = false;
    }
    
    return self;
Error:
    return NULL;
}

- (id) initWithScrolling:(bool)scrolling andFormula:(NSString *)formula andBlock:(bool)block
{
    RESULT r = R_SUCCESS;
    
    m_scrolling = scrolling;
    m_formula = formula;
    m_fBlock = block;
    
    m_xmpNode = NULL;
    
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_INPUT;
    m_Name = @"input";
    
    return self;
Error:
    return NULL;
    
}

-(RESULT)ConstructInput {
    RESULT r = R_SUCCESS;
    
    m_formula = NULL;
    m_scrolling = false;
    m_fBlock = false;
    
    NSString * tempName;
    
    XMPValue inputFormulaVal = [self GetAttributeValueWithName:@"formula"];
    if(inputFormulaVal.m_ValueType != XMP_VALUE_INVALID){
        tempName = [[NSString alloc] initWithCString:inputFormulaVal.GetPszValue() encoding:NSUTF8StringEncoding];
    }else{
        tempName = @"";
    }
    
    m_formula = tempName;
    
    XMPValue inputScrollingVal = [self GetAttributeValueWithName:@"scrolling"];
    if(inputScrollingVal.m_ValueType != XMP_VALUE_INVALID){
        inputScrollingVal.GetValueBool(&m_scrolling);
    }
    
    XMPValue inputBlockVal = [self GetAttributeValueWithName:@"block"];
    if(inputBlockVal.m_ValueType != XMP_VALUE_INVALID){
        inputBlockVal.GetValueBool(&m_fBlock);
    }
    
Error:
    return r;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    if(m_fBlock)
        node->AddAttribute(new XMPAttribute((char *)"block", (char *)"true"));
    else
        node->AddAttribute(new XMPAttribute((char *)"block", (char *)"false"));
    
    if(m_scrolling)
        node->AddAttribute(new XMPAttribute((char *)"scrolling", (char *)"true"));
    else
        node->AddAttribute(new XMPAttribute((char *)"scrolling", (char *)"false"));
    
    node->AddAttribute(new XMPAttribute((char *)"formula", (char *)[m_formula UTF8String]));
    
    
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
