//
//  AudioController.m
//  AudioController
//
//  Created by Idan Beck on 10/2/10.
//  Copyright 2010 Incident Technologies. All rights reserved.
//

#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

#import "AudioController.h"

#import "AUAudioNodeFactory.h"
#import "AUNodeNetwork.h"

#import "CAStreamBasicDescription.h"
#import "CAXException.h"

@implementation AudioController

+(id) sharedAudioController {
    static AudioController *sharedAudioController = NULL;
    static dispatch_once_t onceToken;
    
    dispatch_once(&onceToken, ^{
        sharedAudioController = [[self alloc] init];
    });
    
    return sharedAudioController;
}

- (id) init {
	if(self = [super init]) {
        // Grab the audio session here
        m_session = [AVAudioSession sharedInstance];
        
        [self InitDefaultStreamDescription];
        
        // Activate audio session for good measure
        NSError *activationError = nil;
        BOOL success = [m_session setActive: YES error: &activationError];
        if (!success) {
            NSLog(@"Error: Failed to activate the audio session!");
            m_session = NULL;
        }
        
        // Look at avail routes
        NSLog(@"Lineout Name: %@, Headphones Name: %@, Speaker Name: %@", AVAudioSessionPortLineOut, AVAudioSessionPortHeadphones, AVAudioSessionPortBuiltInSpeaker);
        
        // seed rand for use by all audio generators and effects
        srand(time(NULL));
        
        [AUAudioNodeFactory InitWithAudioController:self];
        [self initializeAUGraph];
        [self RouteAudioToSpeaker]; 
    }
    
    return self;
}

// Default stream description (MONO)
- (AudioStreamBasicDescription*) InitDefaultStreamDescription {
    // Initialize structure to zero
    memset(&m_StreamDefaultDescription, 0, sizeof(AudioStreamBasicDescription));
    
    // Make modifications to the CAStreamBasicDescription
    // Using 16 bit ints to make it easier
    // mixer will accept either 16 bit ints or 32 bit fixed point ints
    m_StreamDefaultDescription.mSampleRate =  AUDIO_CONTROLLER_SAMPLE_RATE;		// sample rate
    m_StreamDefaultDescription.mFormatID = kAudioFormatLinearPCM;               // Format
    m_StreamDefaultDescription.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    m_StreamDefaultDescription.mBitsPerChannel = sizeof(AudioSampleType) * 8;
    m_StreamDefaultDescription.mChannelsPerFrame = 1;
    m_StreamDefaultDescription.mFramesPerPacket = 1;
    m_StreamDefaultDescription.mBytesPerFrame = (m_StreamDefaultDescription.mBitsPerChannel / 8) * m_StreamDefaultDescription.mChannelsPerFrame;
    m_StreamDefaultDescription.mBytesPerPacket = m_StreamDefaultDescription.mBytesPerFrame * m_StreamDefaultDescription.mFramesPerPacket;
    
    return &m_StreamDefaultDescription;
}

- (AudioStreamBasicDescription*)GetDefaultStreamDescription {
    return &m_StreamDefaultDescription;
}

// Returns the internal AUGraph (used by AudioNode)
- (AUGraph*) getAUGraph {
    return (AUGraph*)(&augraph);
}

- (float) getSampleRate {
    return AUDIO_CONTROLLER_SAMPLE_RATE;
}

// Starts the audio render
- (void) startAUGraph {
    Boolean fRunning;
    AUGraphIsRunning(augraph, &fRunning);

    if (fRunning)   // multiple calls to start requires the same number of calls to stop
        return;
	
	// Starts the AUGraph
	OSStatus result = AUGraphStart(augraph);

	if(result)
		printf("AUGraphStart result %d %08X %4.4s\n", (int)result, (int)result, (char*)&result);
	
    // Announce what audio route we are using
    [self AnnounceAudioRouteChange];
    
	return;
}

- (void) HandleAVAudioSessionRouteChange:(NSNotification*)note {
#if TARGET_IPHONE_SIMULATOR
    /* do nothing */
    NSLog(@"HandleAVAudioSessionRouteChange doesn't do anything in simulator");
#else
    // Respond to the route change
    NSLog(@"AVAudioSessionRouteChanged: %@, %@",
          [note.userInfo objectForKey:AVAudioSessionRouteChangeReasonKey],
          [note.userInfo objectForKey:AVAudioSessionRouteChangePreviousRouteKey]);
    
    // Don't let routing go to built in receiver 
    NSString *audioRoute = [self GetNSAudioRoute];
    if ([audioRoute isEqualToString:(NSString*)(AVAudioSessionPortBuiltInReceiver)])
        [self RouteAudioToSpeaker];
    
    [self requestAudioRouteDetails];
    [self AnnounceAudioRouteChange];
#endif
}

void AudioControllerPropertyListener (void *inClientData, AudioSessionPropertyID inID, UInt32 inDataSize, const void *inData) {
#if TARGET_IPHONE_SIMULATOR
    /* do nothing */
    NSLog(@"AudioControllerProperty Listner doesn't do anything in simulator");
#else
    if (inID != kAudioSessionProperty_AudioRouteChange)
        return;
    
    AudioController *ac = (AudioController*)CFBridgingRelease(inClientData);
    
    NSString *audioRoute = [ac GetNSAudioRoute];
    if (![audioRoute isEqualToString:[[NSString alloc] initWithCString:(const char*)(AVAudioSessionPortOverrideSpeaker) encoding:NSUTF8StringEncoding]])
        [ac RouteAudioToSpeaker];
    
    [ac requestAudioRouteDetails];
    [ac AnnounceAudioRouteChange];
#endif
}

// Request that a AudioRouteChange notification get sent out, even though
// no actual change has happened. This is useful for getting the audio
// route state info for initial UI setup.
- (void) requestAudioRouteDetails {
    bool fRouteIsSpeaker = TRUE;
    NSString *audioRoute = [self GetNSAudioRoute];
    
    //CFStringRef newRoute = [self GetAudioRoute];
    //bool routeIsSpeaker = [(NSString*)newRoute isEqualToString:(NSString*)kAudioSessionOutputRoute_BuiltInSpeaker];
    
#if TARGET_IPHONE_SIMULATOR
    fRouteIsSpeaker = TRUE;     // spoof the speaker if it's in simulator
#else
    fRouteIsSpeaker = [audioRoute isEqualToString:(NSString*)(AVAudioSessionPortBuiltInSpeaker)];
#endif
    
    NSDictionary *routeData = [NSDictionary dictionaryWithObjectsAndKeys:
                               [NSNumber numberWithBool:fRouteIsSpeaker], @"isRouteSpeaker",
                               audioRoute, @"routeName", nil];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"AudioRouteChange" object:self userInfo:routeData];
}

// Callback for audio interruption, e.g. a phone call coming in".
void AudioInterruptionListener (void *inClientData, UInt32 inInterruptionState) {
    AudioController *ac = (AudioController *)CFBridgingRelease(inClientData);
    
    // Stop audio graph when an interruption comes in, and restart it when the interruption is done.
    if (inInterruptionState == kAudioSessionBeginInterruption) {
        NSError *errorSetActiveFalse = NULL;
        [[AVAudioSession sharedInstance] setActive:FALSE error:&errorSetActiveFalse];
        if(errorSetActiveFalse != NULL)
            NSLog(@"Failed to set AVAudioSession to not active: %@", [errorSetActiveFalse description]);
        else
            [ac stopAUGraph];
    }
    else if (inInterruptionState == kAudioSessionEndInterruption) {
        NSError *errorSetActiveTrue = NULL;
        [[AVAudioSession sharedInstance] setActive:TRUE error:&errorSetActiveTrue];
        if(errorSetActiveTrue != NULL)
            NSLog(@"Failed to set AVAudioSession to not active: %@", [errorSetActiveTrue description]);
        else
            [ac startAUGraph];
    }
}

- (void) initializeAUGraph {
	OSStatus result = noErr;
    
    if (m_session != NULL) {
        // Set Category
        NSError *setCategoryError = nil;
        BOOL fStatus = [m_session setCategory:AVAudioSessionCategoryPlayAndRecord error:&setCategoryError];
        if(fStatus == FALSE) {
            NSLog(@"AVAudioSession SetCategory failed: %@", [setCategoryError description]);
        }
    }
    else {
        NSLog(@"InitializeAUGraph failed, session not initialized");
        return;
    }
    
    // subscribe for notifications
    [[NSNotificationCenter defaultCenter] addObserver:self
                                          selector:@selector(HandleAVAudioSessionRouteChange:)
                                          name:@"AVAudioSessionRouteChangeNotification" object:NULL];

	// Create the AUGraph
	NewAUGraph(&augraph);
    
    // Create the network node that will run our network
    m_networkNode = (AUNodeNetwork*)[AUAudioNodeFactory MakeAudioNode:AUDIO_NODE_NETWORK];
    
	// Open the graph audio units
	result = AUGraphOpen(augraph);
    
    // Initialize the units
    [m_networkNode Initialize];
	
	// Now call initialize to verify connections
	result = AUGraphInitialize(augraph);
    NSLog(@"AudioController: AUGraphInitialize: %s", CAX4CCString(result).get());
}

// Stops the render
- (void) stopAUGraph {
	Boolean fRunning = false;
	
    // Check to see if graph is running
	AUGraphIsRunning(augraph, &fRunning);
    
    // If multiple AUGraphStart calls were made it will take an equal number
    // of AUGraphStop calls to actually stop the graph.
    while (fRunning) {
        AUGraphStop(augraph);
        AUGraphIsRunning(augraph, &fRunning);
    }
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"AudioEngineStopped" object:self userInfo:nil];
}

- (AUNodeNetwork*) GetNodeNetwork {
    return m_networkNode;
}

- (void) RouteAudioToSpeaker {
    //UInt32 audioRoute = kAudioSessionOverrideAudioRoute_Speaker;
    //AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(audioRoute), &audioRoute);
    
#if TARGET_IPHONE_SIMULATOR
    NSLog(@"Routing not available in simulator");
#else
    NSError *overrideError = NULL;
    [m_session overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker error:&overrideError];
    
    if(overrideError != NULL) {
        NSLog(@"Failed to route audio to speaker: %@", [overrideError description]);
        [self GetAudioRoute];
    }
    else
        [self AnnounceAudioRouteChange];

#endif
}

- (void) RouteAudioToDefault {
    //UInt32 audioRoute = kAudioSessionOverrideAudioRoute_None;
    //AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(audioRoute), &audioRoute);
    
#if TARGET_IPHONE_SIMULATOR
    NSLog(@"Routing not available in simulator");
#else
    NSError *overrideError = NULL;
    [m_session overrideOutputAudioPort:AVAudioSessionPortOverrideNone error:&overrideError];
    
    if(overrideError != NULL) {
        NSLog(@"Failed to route audio to speaker: %@", [overrideError description]);
        [self GetAudioRoute];
    }
    else
        [self AnnounceAudioRouteChange];
#endif
}

- (NSString *) GetNSAudioRoute {
#if TARGET_IPHONE_SIMULATOR
    return @"simulator";
#else
    AVAudioSessionRouteDescription *routeDesc = [m_session currentRoute];
    AVAudioSessionPortDescription *outputPortDesc = [[routeDesc outputs] firstObject];
    if(outputPortDesc != NULL)
    {
        NSLog(@"Current Route: %@", [outputPortDesc portName]);
        return [outputPortDesc portName];
    }
    else
        return NULL;
#endif
}

- (CFStringRef) GetAudioRoute {
    return (CFStringRef)CFBridgingRetain([self GetNSAudioRoute]);
}

- (void) AnnounceAudioRouteChange {
    // Print out the current route
    NSString * routeName = (NSString *)[self GetAudioRoute];
    NSLog(@"Routing audio through %@", routeName);
}

- (void) dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [self stopAUGraph];
	DisposeAUGraph(augraph);
    
	//[super dealloc];
}

@end
