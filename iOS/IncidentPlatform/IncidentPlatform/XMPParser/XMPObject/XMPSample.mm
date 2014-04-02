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

@synthesize Value = m_value;
@synthesize ResourcePath = m_strResourcePath;

@synthesize Starttime = m_starttime;
@synthesize Endtime = m_endttime;
@synthesize SampleRate = m_samplerate;

-(id) initWithXMPNode:(XMPNode*)xmpNode {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = xmpNode;
    CPRM((self = [self init]), "initWithParentLesson: Failed to init");
    m_type = XMP_OBJECT_SAMPLE;
    
    m_SampleBuffer = NULL;
    m_strResourcePath = NULL;
    
    [self ConstructSample];
    
    return self;
Error:
    return NULL;
}

-(id) initWithValue:(int)value path:(NSString*)path {
    RESULT r = R_SUCCESS;
    
    m_xmpNode = NULL;
    CPRM((self = [super init]), "initWithValuePath: Failed to init super");
    
    m_type = XMP_OBJECT_SAMPLE;
    m_Name = @"sample";
    
    m_value = value;
    m_strResourcePath = [[NSString alloc] initWithString:path];
    m_SampleBuffer = NULL;
    
    m_starttime = -1.0f;
    m_endttime = -1.0f;
    m_samplerate = DEFAULT_SAMPLE_RATE;
    
    return self;
Error:
    return NULL;
}

-(RESULT)ConstructSample {
    RESULT r = R_SUCCESS;
    
    float starttime, endtime;
    int samplerate;
    
    if([self HasAttributeWithName:@"fileref"]) {
        NSString *tempPath = [[NSString alloc] initWithCString:[self GetAttributeValueWithName:@"fileref"].GetPszValue()
                                                      encoding:NSUTF8StringEncoding];
        // Check if file exists there
        NSString *componentName = [tempPath stringByDeletingPathExtension];
        NSString *componentExt = [tempPath pathExtension];
        m_strResourcePath = [[NSBundle mainBundle] pathForResource:componentName ofType:componentExt];
    }
    
    [self GetXMPValueOfChild:@"starttime" withAttribute:@"value"].GetValueDouble((double*)(&starttime));
    [self GetXMPValueOfChild:@"endtime" withAttribute:@"value"].GetValueDouble((double*)(&endtime));

    if(m_SampleBuffer != NULL)
        m_SampleBuffer->SetEnd(endtime);
    
    [self GetXMPValueOfChild:@"samplerate" withAttribute:@"value"].GetValueInt((long*)(&samplerate));
    //m_SampleBuffer->m_SampleRate = samplerate;
    
    if([self HasAttributeWithName:@"value"])
        [self GetAttributeValueWithName:@"value"].GetValueInt((long*)(&m_value));
    else
        m_value = -1;
    
    //XMPObject *tempData = [self GetChildWithName:@"data"];
    //int a = 5;
    
Error:
    return r;
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

    // Push over the data
    NSData *bufferData = [NSData dataWithBytes:m_SampleBuffer->GetBufferArray() length:m_SampleBuffer->GetByteSize()];
    NSString *strData = [bufferData base64EncodedStringWithOptions:0];
    
    // Create the data
    xmpTree->AddChildByName("data");
    xmpTree->NavigateToChildName("data");
    xmpTree->AddAttributeByNameValue("datasize", (char *)[[NSString stringWithFormat:@"%lu", m_SampleBuffer->GetByteSize()] UTF8String]);
    xmpTree->AddAttributeByNameValue("dataencoding", "base64");
    xmpTree->AddAttributeByNameValue("length", (char *)[[NSString stringWithFormat:@"%d", [strData length]] UTF8String]);
    
    xmpTree->AppendContent((char *) [strData UTF8String]);
    xmpTree->NavigateToParent();
    
    xmpTree->PrintXMPTree();
    
Error:
    return r;
}

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent {
    XMPNode *node = NULL;
    
    node = new XMPNode((char*)[m_Name UTF8String], parent);
    
    if(m_strResourcePath != NULL)
        node->AddAttribute(new XMPAttribute("fileref", (char*)[m_strResourcePath UTF8String]));
    
    if(m_value >= 0)
        node->AddAttribute(new XMPAttribute("value", m_value));
    
    XMPNode *tempNode = NULL;
    if(m_SampleBuffer != NULL) {
        tempNode = new XMPNode("starttime", node);
        tempNode->AddAttribute(new XMPAttribute("value", (char *)[[NSString stringWithFormat:@"%f", m_SampleBuffer->GetStart()] UTF8String]));
        node->AddChild(tempNode);

        tempNode = new XMPNode("endtime", node);
        tempNode->AddAttribute(new XMPAttribute("value", (char *)[[NSString stringWithFormat:@"%f", m_SampleBuffer->GetEnd()] UTF8String]));
        node->AddChild(tempNode);
        
        tempNode = new XMPNode("samplerate", node);
        tempNode->AddAttribute(new XMPAttribute("value", (char *)[[NSString stringWithFormat:@"%d", m_SampleBuffer->GetSampleRate()] UTF8String]));
        node->AddChild(tempNode);

        // Push over the data
        NSData *bufferData = [NSData dataWithBytes:m_SampleBuffer->GetBufferArray() length:m_SampleBuffer->GetByteSize()];
        NSString *strData = [bufferData base64EncodedStringWithOptions:0];
        tempNode = new XMPNode("data", node);
        tempNode->AddAttribute(new XMPAttribute("datasize", (char *)[[NSString stringWithFormat:@"%lu", m_SampleBuffer->GetByteSize()] UTF8String]));
        tempNode->AddAttribute(new XMPAttribute("dataencoding", "base64"));
        tempNode->AddAttribute(new XMPAttribute("length", (char *)[[NSString stringWithFormat:@"%d", [strData length]] UTF8String]));
        tempNode->AddChild(new XMPNode("content", tempNode, (char *) [strData UTF8String]));
        node->AddChild(tempNode);
    }
    else {
        if(m_starttime >= 0) {
            tempNode = new XMPNode("starttime", node);
            tempNode->AddAttribute(new XMPAttribute("value", (char *)[[NSString stringWithFormat:@"%f", m_starttime] UTF8String]));
            node->AddChild(tempNode);
        }
        
        if(m_endttime >= 0) {
            tempNode = new XMPNode("endtime", node);
            tempNode->AddAttribute(new XMPAttribute("value", (char *)[[NSString stringWithFormat:@"%f", m_endttime] UTF8String]));
            node->AddChild(tempNode);
        }
        
        if(m_samplerate > 0) {
            tempNode = new XMPNode("samplerate", node);
            tempNode->AddAttribute(new XMPAttribute("value", (char *)[[NSString stringWithFormat:@"%d", m_samplerate] UTF8String]));
            node->AddChild(tempNode);
        }
    }
    
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









