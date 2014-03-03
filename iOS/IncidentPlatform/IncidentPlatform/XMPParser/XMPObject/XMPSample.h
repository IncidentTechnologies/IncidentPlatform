//
//  XMPSample.h
//  IncidentPlatform
//
//  Created by Idan Beck on 2/27/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "XMPObject.h"
#import "RESULT.h"

class SampleBuffer;

@interface XMPSample : XMPObject {
    SampleBuffer *m_SampleBuffer;       // contains all the info (including sample)
}

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithSampleBuffer:(SampleBuffer*)sampleBuffer;

-(RESULT) CreateXMPTreeAndSaveToFile:(char *)pszFilename andOverwrite:(BOOL)fOverwrite;

@end
