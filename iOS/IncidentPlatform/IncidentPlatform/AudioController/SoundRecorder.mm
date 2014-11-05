//
//  SoundRecorder.m
//  IncidentPlatform
//
//  Created by Idan Beck on 11/3/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "SoundRecorder.h"
#include <stdio.h>

@implementation SoundRecorder {
    
}

@synthesize delegate;
@synthesize avAudioRecorder;

- (id)init {
    self = [super init];
    
    if (self) {
        defaultFilename = @"tempSoundRecording.wav";
        NSArray * pathComponents = [NSArray arrayWithObjects:[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject], defaultFilename, nil];
        NSURL * outputFileURL = [NSURL fileURLWithPathComponents:pathComponents];
        NSLog(@"Output file URL is %@", outputFileURL);
        
        // Setup audio session
        AVAudioSession * session = [AVAudioSession sharedInstance];
        [session setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
        
        // Define recorder setting
        NSMutableDictionary * recordSetting = [[NSMutableDictionary alloc] init];
        
        [recordSetting setValue:[NSNumber numberWithInt:kAudioFormatLinearPCM] forKey:AVFormatIDKey];
        [recordSetting setValue:[NSNumber numberWithFloat:44100.0] forKey:AVSampleRateKey];
        [recordSetting setValue:[NSNumber numberWithInt:1] forKey:AVNumberOfChannelsKey];
        [recordSetting setValue:[NSNumber numberWithInt:16] forKey:AVLinearPCMBitDepthKey];
        [recordSetting setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsFloatKey];
        [recordSetting setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsBigEndianKey];
        [recordSetting setValue:[NSNumber numberWithInt:AVAudioQualityHigh] forKey:AVEncoderAudioQualityKey];
        
        // Initiate and prepare the recorder
        avAudioRecorder = [[AVAudioRecorder alloc] initWithURL:outputFileURL settings:recordSetting error:NULL];
        avAudioRecorder.delegate = self;
        avAudioRecorder.meteringEnabled = YES;
        
        [avAudioRecorder prepareToRecord];
    }
    
    return self;
}

#pragma mark - Recording

-(void) Start {
    if(!avAudioRecorder.recording){
        NSLog(@"Recording began");
        
        AVAudioSession *session = [AVAudioSession sharedInstance];
        [session setActive:YES error:nil];
        
        // Start recording
        [avAudioRecorder record];
        
        if([self delegate] != NULL)
            [delegate OnRecordStart];
    }
    else {
        [self Pause];
    }
}

-(void) Pause {
    if(avAudioRecorder.recording){
        NSLog(@"Recording paused");
        [avAudioRecorder pause];
    }
    else {
        [self Start];
    }
}

-(void) Stop {
    NSLog(@"Recording stopped");
    
    if(avAudioRecorder.recording){
        [avAudioRecorder stop];
        
        AVAudioSession *session = [AVAudioSession sharedInstance];
        [session setActive:NO error:nil];
    }
}

-(BOOL)isRecording {
    return avAudioRecorder.recording;
}


#pragma mark - Audio Recorder Delegate

- (void)audioRecorderDidFinishRecording:(AVAudioRecorder *)recorder successfully:(BOOL)flag {
    if(flag) {
        NSLog(@"Recording finished with success");
        
        if([self delegate] != NULL) {
            // Get the filepath of the temp file
            char * filepath = (char *)malloc(sizeof(char) * 1024);
            NSArray * paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            NSString * path = [paths objectAtIndex:0];
            NSString * sampleFilename = [path stringByAppendingPathComponent:defaultFilename];
            filepath = (char *) [sampleFilename UTF8String];
            
            // Create a sample node from it and give to the callback
            SampleNode *sampNode = new SampleNode(filepath);
            [delegate OnRecordEnd:sampNode];
        }
        
    }
    else {
        NSLog(@"Recording finished with no success");
    }
}


#pragma mark - File System

- (void) SaveRecordingToFilename:(NSString *)filename {
    NSLog(@"Save Recording to Filename %@", filename);
    
    BOOL useBundle = TRUE;
    NSString * newPath;
    
    if(useBundle){
        // *** Save Bundle Sound To File
        newPath = [[NSBundle mainBundle] pathForResource:filename ofType:@"wav"];
        NSLog(@"Using Main Bundle Filepath %@", newPath);
    }
    else {
        // **** Save Recorded Sound To File
        // Save editing changes to file (CustomSoundPlaceholder.wav)
        NSArray * paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString * path = [paths objectAtIndex:0];
        newPath = [path stringByAppendingPathComponent:defaultFilename];
        
        NSLog(@"Using Custom Sound Filepath %@", newPath);
        
        char * pathName = (char *)malloc(sizeof(char) * [newPath length]);
        pathName = (char *) [newPath UTF8String];
        
        //m_sampNode->SaveToFile(pathName, YES);
    }
    
    // Then get data and upload
    NSData *data = [[NSData alloc] initWithContentsOfFile:newPath];
    
    //NSSample *xmpSample = [[NSSample alloc] initWithName:[NSString stringWithFormat:@"%@.wav",filename] custom:YES value:@"0" xmpFileId:0];
    //[g_ophoMaster saveSample:xmpSample withFile:data];
    
    // Delete after recording
    [self DeleteRecordingFilename:defaultFilename];
    
    // Still need this?
    //[self ReleaseAudioBank];
}

- (void)DeleteRecordingFilename:(NSString *)filename {
    if(filename != nil && [filename length] > 0) {
        // Create a subfolder Samples/{Category} if it doesn't exist yet
        NSLog(@"Deleting file %@.wav",filename);
        
        NSString * newFilename = filename;
        //newFilename = [@"Samples/Custom_" stringByAppendingString:filename];
        //newFilename = [newFilename stringByAppendingString:@".wav"];
        
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSError * err = NULL;
        NSFileManager * fm = [[NSFileManager alloc] init];
        NSString * newPath = [[paths objectAtIndex:0] stringByAppendingPathComponent:newFilename];
        
        BOOL result = [fm removeItemAtPath:newPath error:&err];
        
        if(!result)
            NSLog(@"Error deleting");
    }
    else {
        NSLog(@"Trying to delete a nil file");
    }
    
}

#pragma mark - Audio Controller Sampler

- (void)InitAudioForSample {
    
    // TODO
    
    // Reload sound into bank after new record
    char * filepath = (char *)malloc(sizeof(char) * 1024);
    
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString * path = [paths objectAtIndex:0];
    NSString * sampleFilename = [path stringByAppendingPathComponent:defaultFilename];
    
    filepath = (char *) [sampleFilename UTF8String];
}

-(void)finishAudioInitAfterBufferFlush {
    /* empty stub */
}

/*
- (float) GetSampleRelativeLength {
    float sampleRelativeLength = m_sampleEnd - m_sampleStart;
    
    NSLog(@"sampleRelativeLength is now %f with sampleEnd %f and sampleStart %f", sampleRelativeLength, m_sampleEnd, m_sampleStart);
    
    if(sampleRelativeLength <= 0)
        return [self GetSampleLength] - m_sampleStart;
    else
        return sampleRelativeLength;
}
 */


@end
