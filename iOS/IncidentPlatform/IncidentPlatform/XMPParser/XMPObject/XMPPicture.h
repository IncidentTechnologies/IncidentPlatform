//
//  XMPPicture.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 6/18/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"

@interface XMPPicture : XMPObject {
    
    NSString *m_pictureId;
    NSString *m_filepath;
    
    double m_originX;
    double m_originY;
    double m_sizeWidth;
    double m_sizeHeight;
    
    int m_layer;
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithId:(NSString *)pictureId Filepath:(NSString *)filePath OriginX:(double)x OriginY:(double)y SizeWidth:(double)width SizeHeight:(double)height Layer:(int)layer;

-(RESULT)ConstructPicture;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
