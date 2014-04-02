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
    SampleBuffer *m_SampleBuffer;       // Contains all the info (including sample)
}

@property int Value;
@property NSString* ResourcePath;

// These only used if SampleBuffer is NULL and they're positive
@property double Starttime;
@property double Endtime;
@property double SampleRate;

-(id) initWithXMPNode:(XMPNode*)xmpNode;
-(id) initWithValue:(int)value path:(NSString*)path;
-(id) initWithSampleBuffer:(SampleBuffer*)sampleBuffer;

-(RESULT) CreateXMPTreeAndSaveToFile:(char *)pszFilename andOverwrite:(BOOL)fOverwrite;
-(RESULT) ConstructSample;

-(XMPNode*)CreateXMPNodeFromObjectWithParent:(XMPNode*)parent;

@end
