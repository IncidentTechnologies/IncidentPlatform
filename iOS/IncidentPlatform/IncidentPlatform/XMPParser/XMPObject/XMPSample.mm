//
//  XMPSample.m
//  IncidentPlatform
//
//  Created by Idan Beck on 2/27/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPSample.h"
#import "SampleNode.h"
#import "XMPTree.h"

@implementation XMPSample

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    XMPValue xmpValString;
    XMPValue xmpValFret;
    NSString *tempString = NULL;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_SAMPLE;
    
    // Create the sample buffer here from the data (base64 encoding)
    
    return self;
Error:
    return NULL;
}

-(id) initWithSampleBuffer:(SampleBuffer*)sampleBuffer {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = new XMPNode("sample");
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_SAMPLE;
    
    // Assign the buffer
    m_SampleBuffer = sampleBuffer;
    
    return self;
Error:
    return NULL;
}

-(RESULT) CreateXMPTreeAndSaveToFile:(char *)pszFilename andOverwrite:(BOOL)fOverwrite {
    RESULT r = R_SUCCESS;
    
    // First create an empty XMP tree
    XMPTree *xmpTree = new XMPTree();
    
    // Create XMP tag
    xmpTree->AddChildByName("xmp");
    xmpTree->NavigateToChildName("xmp");
    
    // Create Sample tag
    xmpTree->AddChildByName("sample");
    xmpTree->NavigateToChildName("sample");
    
    xmpTree->AddChildByName("starttime");
    xmpTree->NavigateToChildName("starttime");
    xmpTree->AddAttributeByNameValue("value", (char *)[[NSString stringWithFormat:@"%f", m_SampleBuffer->GetStart()] UTF8String]);
    xmpTree->NavigateToParent();
    
    xmpTree->AddChildByName("endtime");
    xmpTree->NavigateToChildName("endtime");
    xmpTree->AddAttributeByNameValue("value", (char *)[[NSString stringWithFormat:@"%f", m_SampleBuffer->GetEnd()] UTF8String]);
    xmpTree->NavigateToParent();
    
    xmpTree->AddChildByName("samplerate");
    xmpTree->NavigateToChildName("samplerate");
    xmpTree->AddAttributeByNameValue("value", (char *)[[NSString stringWithFormat:@"%d", m_SampleBuffer->GetSampleRate()] UTF8String]);
    xmpTree->NavigateToParent();
    
    // Create the data
    xmpTree->AddChildByName("data");
    xmpTree->NavigateToChildName("data");
    xmpTree->AddAttributeByNameValue("datasize", (char *)[[NSString stringWithFormat:@"%lu", m_SampleBuffer->GetByteSize()] UTF8String]);
    xmpTree->AddAttributeByNameValue("dataencoding", "base64");

    // push over the data
    NSData *bufferData = [NSData dataWithBytes:m_SampleBuffer->GetBufferArray() length:m_SampleBuffer->GetByteSize()];
    xmpTree->AppendContent((char *)[[bufferData base64EncodedStringWithOptions:0] UTF8String]);
    xmpTree->NavigateToParent();
    
    xmpTree->PrintXMPTree();
    
Error:
    return r;
}

@end









