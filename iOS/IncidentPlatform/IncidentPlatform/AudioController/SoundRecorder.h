//
//  SoundRecorder.h
//  IncidentPlatform
//
//  Created by Idan Beck on 11/3/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#include "EHM.h"

#include "SampleNode.h"

@protocol SoundRecorderDelegate <NSObject>
    - (void) OnRecordStart;
    - (void) OnRecordEnd:(SampleNode*)sampleNode;
@end

@interface SoundRecorder : NSObject <AVAudioRecorderDelegate> {
    NSString * defaultFilename;
    
    float m_timePaused;
    float m_sampleStart;
    float m_sampleEnd;
}

- (id) init;
- (void) Start;
- (void) Pause;
- (void) Stop;
- (void) Clear;
-(BOOL) isRecording;

- (void) SaveRecordingToFilename:(NSString *)filename;
- (void) DeleteRecordingFilename:(NSString *)filename;
- (void) InitAudioForSample;

- (void) ReleaseAudio;

@property (retain) AVAudioRecorder *avAudioRecorder;
@property (weak, nonatomic) id<SoundRecorderDelegate> delegate;

@end
