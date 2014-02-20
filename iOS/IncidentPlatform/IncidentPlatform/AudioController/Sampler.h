//
//  Sampler.h
//  gTarAudioController
//
//  Created by Franco Cedano on 1/10/12.
//  Copyright (c) 2012 Incident Technologies. All rights reserved.
//

#import <AudioToolbox/AudioToolbox.h>

// Data structure for mono or stereo sound, to pass to the application's render callback function, 
//    which gets invoked by a Mixer unit input bus when it needs more audio to play.
typedef struct
{
    
    BOOL                 isStereo;           // set to true if there is data in the audioDataRight member
    UInt32               frameCount;         // the total number of frames in the audio data
    AudioSampleType  *audioDataLeft;     // the complete left (or mono) channel of audio data read from an audio file
    AudioSampleType  *audioDataRight;    // the complete right channel of audio data read from an audio file
    
} soundStruct, *soundStructPtr;

@interface Sampler : NSObject
{
    int m_sampleRate;
    
    CFURLRef *m_sampleNameArray;
    // this array can be though of as a keyboard, holding one sample for each note
    soundStruct *m_soundStructArray;
    // an array representing each fret position on a guitar, each fret position points
    // to a note/sample to be played
    soundStruct *m_soundMappingArray[6][17];
    // an array to keep track of the position with in a sample each string should play. Since multiple
    // fret positions can point to the same sample, this is needed inorder so allow playing the same
    // note/sample on different strings
    int m_sampleNumber[6];
    
    // 2d array indicating which fret positions are pushed down. Index 0 for the 2nd dimention of the
    // array corresponds to open string, it should never be down since it's not an actual fret.
    bool m_fretsPressedDown[6][17];
    
    // Array indicating which fret position should currently be played on each string. A value of -1
    // indicates nothing should be played on that string.
    int m_fretToPlay[6];
    
    //
    int m_pendingFretToPlay[6];
    int m_pendingFretToPlayStartIndex[6];
    
    // The volume and attenuation of each string [0 1], different volume/attenuation per string.
    // Volume is how loud the sample plays, attenuation is how fast the volume decreases. With a
    // volume = 1, attenuation = 0, the sample will be played back as recorded.
    float m_volume[6];
    float m_attenuation[6];
    
    // used to detect if there is a new load request pending. If
    // a thread is in the middle of loading and there is 1 or more pending request, it
    // should stop loading and exit the syncronization block to allow,
    // the new more recent load to be fulfilled. 
    int m_pendingLoadRequests;
    
    BOOL m_pendingLoad;
}

@property (readwrite) AudioStreamBasicDescription monoStreamFormat;
@property (readwrite) AudioStreamBasicDescription stereoStreamFormat;

@property (retain) NSArray *m_instruments;
@property (copy) NSString *m_samplePackName;
@property int m_currentSamplePackIndex;
@property int m_firstNoteMidiNum;
@property int m_numberOfSamples;
@property (retain) NSArray *m_tuning;
@property (retain) NSArray *m_standardTunning;

- (id) initWithSampleRate:(int)sampleRate AndSamplePack:(NSString*)name;

- (bool) loadSamplerWithName:(NSString*)name;
- (bool) loadSamplerWithIndex:(int)index;
- (void) asynchLoadSamplerWithIndex:(int)index withSelector:(SEL)aSelector andOwner:(NSObject*)parent;
- (void) asynchLoadSamplerWithName:(NSString*)name withSelector:(SEL)aSelector andOwner:(NSObject*)parent;
- (void) finishedLoadingSamplePack:(BOOL)result withSelector:(SEL)aSelector andOwner:(NSObject*)parent;

- (bool) loadInstrumentArray;
- (int) getSamplePackNumFromName:(NSString*)name;
- (bool) loadSamplePackMetaData:(int)samplePackNum;
- (void) obtainSoundFileURLs;
- (void) setupMappingArray;
- (void) setupMonoStreamFormat;
- (void) setupStereoStreamFormat;
- (bool) readAudioFilesIntoMemory;

- (void) PluckString:(int)string atFret:(int)fret withAmplitude:(float)amplitude;
- (void) PluckMutedString:(int)string;
- (float) getNextSample;
- (void) Reset;

- (void) FretDown:(int)fret onString:(int)string;
- (void) FretUp:(int)fret onString:(int)string;

- (void) noteOffAtString:(int)string andFret:(int)fret;

- (NSArray*) getInstrumentNames;
- (int) getCurrentSamplePackIndex;

- (int) getNextPositiveZeroCrossingSampleForString:(int)string atFret:(int)fret afterSampleIndex:(int)sampleIndex;

- (void) printErrorMessage: (NSString *) errorString withStatus: (OSStatus) result;
- (void) releaseAudioData;

@end
