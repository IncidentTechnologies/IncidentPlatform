//
//  XMPPicture.m
//  IncidentPlatform
//
//  Created by Kate Schnippering on 6/18/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPPicture.h"

@implementation XMPPicture

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [super init]), "initWithXMPNode: Failed to init super");
    m_type = XMP_OBJECT_PICTURE;
    
    //CRM([self ConstructPicture], "initWithXMPNode: Failed to construct from XMP");
    
    return self;
Error:
    return NULL;
}

-(id) initWithId:(NSString *)pictureId Filepath:(NSString *)filePath OriginX:(double)x OriginY:(double)y SizeWidth:(double)width SizeHeight:(double)height Layer:(int)layer
{
    RESULT r = R_SUCCESS;
    
    m_xmpNode = NULL;
    CPRM((self = [super init]), "initWithParentLesson: Failed to init super");
    
    m_type = XMP_OBJECT_PICTURE;
    m_Name = @"picture";
    
    m_pictureId = [[NSString alloc] initWithString:pictureId];
    m_filepath = [[NSString alloc] initWithString:filePath];
    
    m_originX = x;
    m_originY = y;
    m_sizeWidth = width;
    m_sizeHeight = height;
    
    m_layer = layer;
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructPicture {
    RESULT r = R_SUCCESS;
    
    XMPValue idVal = [self GetAttributeValueWithName:@"id"];
    if(idVal.m_ValueType != XMP_VALUE_INVALID)
        m_pictureId = [[NSString alloc] initWithCString:idVal.GetPszValue() encoding:NSUTF8StringEncoding];
    else
        m_pictureId = @"";
    
    XMPValue filepathVal = [self GetAttributeValueWithName:@"filepath"];
    if(filepathVal.m_ValueType != XMP_VALUE_INVALID)
        m_filepath = [[NSString alloc] initWithCString:filepathVal.GetPszValue() encoding:NSUTF8StringEncoding];
    else
        m_filepath = @"";
    
    if([self HasAttributeWithName:@"x"])
        [self GetAttributeValueWithName:@"x"].GetValueDouble(&m_originX);
    else
        m_originX = 0.0f;
    
    if([self HasAttributeWithName:@"y"])
        [self GetAttributeValueWithName:@"y"].GetValueDouble(&m_originY);
    else
        m_originY = 0.0f;
    
    if([self HasAttributeWithName:@"width"])
        [self GetAttributeValueWithName:@"width"].GetValueDouble(&m_sizeWidth);
    else
        m_sizeWidth = 0.0f;
    
    if([self HasAttributeWithName:@"height"])
        [self GetAttributeValueWithName:@"height"].GetValueDouble(&m_sizeHeight);
    else
        m_sizeHeight = 0.0f;
    
    if([self HasAttributeWithName:@"layer"])
        [self GetAttributeValueWithName:@"layer"].GetValueInt((long *)&m_layer);
    else
        m_layer = 0.0f;
    
Error:
    return r;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    node->AddAttribute(new XMPAttribute((char *)"id", (char*)[m_pictureId UTF8String]));
    
    node->AddAttribute(new XMPAttribute((char *)"filepath", (char*)[m_filepath UTF8String]));
    
    node->AddAttribute(new XMPAttribute((char *)"x", m_originX));
    node->AddAttribute(new XMPAttribute((char *)"y", m_originY));
    
    node->AddAttribute(new XMPAttribute((char *)"width", m_sizeWidth));
    node->AddAttribute(new XMPAttribute((char *)"height", m_sizeHeight));
    
    node->AddAttribute(new XMPAttribute((char *)"layer", m_layer));
    
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
