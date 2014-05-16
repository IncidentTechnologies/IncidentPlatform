//
// SampleNode.cpp
// IncidentPlatform
//
// Created by Idan Beck on 2/17/14.
// Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "SampleNode.h"
#include "CAXException.h"
#include <AudioToolbox/AudioFormat.h>

SampleBuffer::SampleBuffer(char *pszFilenamePath) :
m_pBuffer_c(0),
m_pBuffer(NULL),
m_pBuffer_n(0),
m_SampleRate(DEFAULT_SAMPLE_RATE),
m_pBuffer_start(0),
m_pBuffer_end(0)
{
    LoadSampleBufferFromPath(pszFilenamePath);
}

SampleBuffer::~SampleBuffer() {
    if(m_pBuffer != NULL) {
        free(m_pBuffer);
        m_pBuffer = NULL;
    }
}

inline bool SampleBuffer::SampleDone() {
    return (m_pBuffer_c >= m_pBuffer_end);
}

RESULT SampleBuffer::ResetSampleCounter() {
    m_pBuffer_c = m_pBuffer_start;
    return R_SUCCESS;
}

inline float SampleBuffer::GetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;
    
    if(m_pBuffer != NULL && m_pBuffer_n > 0 && m_pBuffer_end > 0 && m_pBuffer_c < m_pBuffer_end) {
        retVal = m_pBuffer[m_pBuffer_c];
        m_pBuffer_c++;
    }
    
    return retVal;
}

float SampleBuffer::GetSampleBufferLengthMS() {
    return ((float)m_pBuffer_n / (float)m_SampleRate) * 1000.0f;
}

RESULT SampleBuffer::SetStart(float msStart) {
    RESULT r = R_SUCCESS;
    
    float sampleLengthMs = GetSampleBufferLengthMS();
    unsigned long newStart;
    
    CBRM((msStart > 0.0f), "SampleBuffer: Sample start cannot be negative");
    CBRM((msStart < sampleLengthMs), "SampleBuffer: Sample start cannot be longer than sample");
    
    newStart = (msStart / 1000.0f) * (float)m_SampleRate;
    CRM(SetStartSample(newStart), "SampleBuffer: SetStartSample failed");
    
Error:
    return r;
}

RESULT SampleBuffer::SetEnd(float msEnd){
    RESULT r = R_SUCCESS;
    
    float sampleLengthMs = GetSampleBufferLengthMS();
    unsigned long newEnd;
    
    CBRM((msEnd > 0.0f), "SampleBuffer: Sample end cannot be negative");
    CBRM((msEnd <= sampleLengthMs), "SampleBuffer: Sample end cannot be longer than sample");
    
    newEnd = (msEnd / 1000.0f) * (float)m_SampleRate;
    CRM(SetEndSample(newEnd), "SampleBuffer: SetEndSample failed");
    
Error:
    return r;
}

// Warning, this can be dangerous to manipulate
RESULT SampleBuffer::SetSampleRate(unsigned long samplerate) {
    m_SampleRate = samplerate;
    return R_SUCCEED;
}

float SampleBuffer::GetStart() {
    return ((float)m_pBuffer_start * 1000.0f) / (float)m_SampleRate;
}

float SampleBuffer::GetEnd() {
    return ((float)m_pBuffer_end * 1000.0f) / (float)m_SampleRate;
}

int SampleBuffer::GetSampleRate() {
    return m_SampleRate;
}

RESULT SampleBuffer::SetTime(float msTime) {
    float newTimeSample = (msTime / 1000.0f) * (float)m_SampleRate;
    return SetSample(newTimeSample);
}

RESULT SampleBuffer::SetStartSample(unsigned long start) {
    RESULT r = R_SUCCESS;
    
    CBRM((start < m_pBuffer_n), "SampleBuffer: Sample start sample cannot be more than buffer length");
    CBRM((start < m_pBuffer_end), "SampleBuffer: Sample start after sample end");
    
    m_pBuffer_start = start;
    
Error:
    return r;
}

RESULT SampleBuffer::SetEndSample(unsigned long end) {
    RESULT r = R_SUCCESS;
    
    CBRM((end < m_pBuffer_n), "SampleBuffer: Sample end sample cannot be more than buffer length");
    CBRM((end > m_pBuffer_start), "SampleBuffer: Sample end before sample start");
    
    m_pBuffer_end = end;
    
Error:
    return r;
}

RESULT SampleBuffer::SetSample(unsigned long sample) {
    RESULT r = R_SUCCESS;
    
    CBRM((sample < m_pBuffer_n), "SampleBuffer: Sample cannot be more than buffer length");
    CBRM((sample > m_pBuffer_start), "SampleBuffer: Sample before sample start");
    CBRM((sample < m_pBuffer_end), "SampleBuffer: Sample after sample section end");
    
    m_pBuffer_c = sample;
    
Error:
    return r;
}

unsigned long int SampleBuffer::GetByteSize() {
    unsigned long int byteSize = m_pBuffer_n * sizeof(float);
    return byteSize;
}

void* SampleBuffer::GetBufferArray() {
    return (void*)m_pBuffer;
}

unsigned long SampleBuffer::GetSampleCount() {
    return m_pBuffer_n;
}

// Slightly different than Sample Count, this will count the start/end samples
unsigned long SampleBuffer::GetStartStopSampleCount() {
    return m_pBuffer_end - m_pBuffer_start;
}

// TODO: Add Stereo
AudioStreamBasicDescription SampleBuffer::GetClientFormatDescription(bool fStereo) {
    AudioStreamBasicDescription ClientDataFormat;
    memset(&ClientDataFormat, 0, sizeof(AudioStreamBasicDescription));
    
    size_t bytesPerSample = sizeof (AudioSampleType);
    
    ClientDataFormat.mFormatID = kAudioFormatLinearPCM;
    ClientDataFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger;
    ClientDataFormat.mChannelsPerFrame = 1; // 1 indicates mono
    ClientDataFormat.mBytesPerFrame = bytesPerSample; //ClientDataFormat.mChannelsPerFrame * sizeof(float);
    ClientDataFormat.mFramesPerPacket = 1;
    ClientDataFormat.mBytesPerPacket = bytesPerSample; //ClientDataFormat.mBytesPerFrame * ClientDataFormat.mFramesPerPacket;
    ClientDataFormat.mBitsPerChannel = 8 * bytesPerSample; //8 * sizeof(float);
    ClientDataFormat.mSampleRate = m_SampleRate;
    
    return ClientDataFormat;
}

Boolean IsAACHardwareEncoderAvailable() {
    Boolean isAvailable = false;
    
    // get an array of AudioClassDescriptions for all installed encoders for the given format
    // the specifier is the format that we are interested in - this is 'aac ' in our case
    UInt32 encoderSpecifier = kAudioFormatMPEG4AAC;
    UInt32 size;
    
    OSStatus status = AudioFormatGetPropertyInfo(kAudioFormatProperty_Encoders,
                                                 sizeof(encoderSpecifier),
                                                 &encoderSpecifier,
                                                 &size);
    if (status) {
        DEBUG_LINEOUT("AudioFormatGetPropertyInfo kAudioFormatProperty_Encoders error %lu %4.4s\n", status, CAX4CCString(status).get());
        return false;
    }
    
    UInt32 numEncoders = size / sizeof(AudioClassDescription);
    AudioClassDescription encoderDescriptions[numEncoders];
    
    status = AudioFormatGetProperty(kAudioFormatProperty_Encoders,
                                    sizeof(encoderSpecifier),
                                    &encoderSpecifier,
                                    &size,
                                    encoderDescriptions);
    if (status) {
        DEBUG_LINEOUT("AudioFormatGetProperty kAudioFormatProperty_Encoders error %lu %4.4s\n", status, CAX4CCString(status).get());
        return false;
    }
    
    for (UInt32 i=0; i < numEncoders; ++i)
        if (encoderDescriptions[i].mSubType == kAudioFormatMPEG4AAC && encoderDescriptions[i].mManufacturer == kAppleHardwareAudioCodecManufacturer)
            isAvailable = true;
    
    return isAvailable;
}

// Currentlt configured for m4a
RESULT SampleBuffer::SaveToFile(char *pszFilepath, bool fOverwrite) {
    RESULT r = R_SUCCEED;
    
    ExtAudioFileRef outfileRef;
    
    CFStringRef cfstrPath = CFStringCreateWithCStringNoCopy(NULL, pszFilepath, kCFStringEncodingMacRoman, NULL);
    CFURLRef outputFileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                                                           cfstrPath,
                                                           kCFURLPOSIXPathStyle,
                                                           false);
    
    // Create the file
    AudioStreamBasicDescription LocalDataFormat = GetClientFormatDescription(false);
    
    AudioFileTypeID fileType = kAudioFileM4AType;
    
    AudioStreamBasicDescription OutputDataFormat;
    memset(&OutputDataFormat, 0, sizeof(AudioStreamBasicDescription));
    
    OutputDataFormat.mSampleRate = 44100.0;
    OutputDataFormat.mFormatID = kAudioFormatMPEG4AAC;
    OutputDataFormat.mFormatFlags = kMPEG4Object_AAC_Main;
    OutputDataFormat.mChannelsPerFrame = 1;
    OutputDataFormat.mBytesPerPacket = 0;
    OutputDataFormat.mBytesPerFrame = 0;
    OutputDataFormat.mFramesPerPacket = 1024;
    OutputDataFormat.mBitsPerChannel = 0;
    OutputDataFormat.mReserved = 0;
    
    
    UInt32 flags = (fOverwrite) ? kAudioFileFlags_EraseFile : 0;
    OSStatus status = ExtAudioFileCreateWithURL(outputFileURL,
                                                fileType,
                                                &OutputDataFormat,
                                                NULL,
                                                flags,
                                                &outfileRef);
    if(status) {
        DEBUG_LINEOUT("SampleBuffer: SaveToFile: ExtAudioFileCreate failed: %s", CAX4CCString(status).get());
    }
    
    // Specify Codec
    // TODO: Some issue here, not sure what since this still fails when the encoder is present
    //UInt32 codec = (IsAACHardwareEncoderAvailable()) ? kAppleHardwareAudioCodecManufacturer : kAppleSoftwareAudioCodecManufacturer;
    UInt32 codec = kAppleSoftwareAudioCodecManufacturer;
    status = ExtAudioFileSetProperty(outfileRef,
                                     kExtAudioFileProperty_CodecManufacturer,
                                     sizeof(codec),
                                     &codec);
    
    if(status) {
        DEBUG_LINEOUT("SampleBuffer: SaveToFile: ExtAudioFileSetProperty:kExtAudioFileProperty_CodecManufacturer failed: %s", CAX4CCString(status).get());
    }
    
    // Tell the ExtAudioFile API what format we'll be sending samples in
    //LocalDataFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked | kAudioFormatFlagsNativeEndian;
    //LocalDataFormat.mChannelsPerFrame = 2;
    
    status = ExtAudioFileSetProperty(outfileRef,
                                     kExtAudioFileProperty_ClientDataFormat,
                                     sizeof(AudioStreamBasicDescription),
                                     &LocalDataFormat);
    
    if (status) {
        DEBUG_LINEOUT("SampleBuffer: SaveToFile: ExtAudioFileSetProperty failed: %s", CAX4CCString(status).get());
    }
    
    // Create the audio buffer and copy
    AudioSampleType *pAudioSampleBuffer;
    pAudioSampleBuffer = (AudioSampleType*)malloc(sizeof(AudioSampleType) * GetStartStopSampleCount());
    
    for(int i = m_pBuffer_start; i < m_pBuffer_end; i++) {
        pAudioSampleBuffer[i - m_pBuffer_start] = m_pBuffer[i] * 32767.0f;
    }
    
    // Set up the audio bufer list
    AudioBufferList *pAudioData = new AudioBufferList();
    pAudioData->mNumberBuffers = 1;
    pAudioData->mBuffers[0].mNumberChannels = LocalDataFormat.mChannelsPerFrame;
    pAudioData->mBuffers[0].mDataByteSize = GetStartStopSampleCount() * LocalDataFormat.mBytesPerFrame; //totalFramesInFile * srcFormat.mBytesPerFrame;
    pAudioData->mBuffers[0].mData = pAudioSampleBuffer;
    
    UInt32 numberOfFramesToWrite = (UInt32) GetStartStopSampleCount();
    status = ExtAudioFileWrite (outfileRef,
                                numberOfFramesToWrite,
                                pAudioData);
    
    if(status) {
        DEBUG_LINEOUT("SampleBuffer: SaveToFile: ExtAudioFileWrite failed: %s", CAX4CCString(status).get());
    }
    
    // close the file
    status = ExtAudioFileDispose(outfileRef);
    if(status) {
        DEBUG_LINEOUT("SampleBuffer: SaveToFile: ExtAudioFileDispose failed: %s", CAX4CCString(status).get());
    }
    
    // Kill the AudioBufferList
    if(pAudioData != NULL) {
        delete pAudioData;
        pAudioData = NULL;
    }
    
    if(pAudioSampleBuffer != NULL) {
        free(pAudioSampleBuffer);
        pAudioSampleBuffer = NULL;
    }
    
Error:
    return r;
}

RESULT SampleBuffer::LoadSampleBufferFromPath(char *pszPath) {
    RESULT r = R_SUCCESS;
    
    // Get the file reference
    ExtAudioFileRef fileRef;
    CFStringRef cfstrPath = CFStringCreateWithCStringNoCopy(NULL, pszPath, kCFStringEncodingMacRoman, NULL);
    CFURLRef inputFileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                                                          cfstrPath,
                                                          kCFURLPOSIXPathStyle,
                                                          false);
    //CFRelease(cfstrPath);
    OSStatus status = ExtAudioFileOpenURL(inputFileURL, &fileRef);
    
    // Source Format
    AudioStreamBasicDescription srcFormat;
    UInt32 srcFormatSize = sizeof(srcFormat);
    ExtAudioFileGetProperty(fileRef,
                            kExtAudioFileProperty_FileDataFormat,
                            &srcFormatSize,
                            &srcFormat);
    
    // Get the audio file's length in frames.
    UInt64 totalFramesInFile = 0;
    UInt32 frameLengthPropertySize = sizeof (totalFramesInFile);
    status = ExtAudioFileGetProperty ( fileRef,
                                      kExtAudioFileProperty_FileLengthFrames,
                                      &frameLengthPropertySize,
                                      &totalFramesInFile
                                      );
    // Set up the buffer from the length
    m_pBuffer_n = (unsigned long int)(totalFramesInFile);
    m_pBuffer_start = 0;
    m_pBuffer_end = m_pBuffer_n;
    
    AudioSampleType *pAudioSampleBuffer;
    pAudioSampleBuffer = (AudioSampleType*)malloc(sizeof(AudioSampleType) * m_pBuffer_n);
    
    // Set the client format description
    AudioStreamBasicDescription ClientDataFormat = GetClientFormatDescription(false);
    status = ExtAudioFileSetProperty (fileRef,
                                      kExtAudioFileProperty_ClientDataFormat,
                                      sizeof (AudioStreamBasicDescription),
                                      &ClientDataFormat
                                      );
    
    // Set up the AudioBufferList to receive the data, and read it in
    AudioBufferList *pAudioData = new AudioBufferList();
    pAudioData->mNumberBuffers = 1;
    pAudioData->mBuffers[0].mNumberChannels = srcFormat.mChannelsPerFrame;
    pAudioData->mBuffers[0].mDataByteSize = totalFramesInFile * ClientDataFormat.mBytesPerFrame; //totalFramesInFile * srcFormat.mBytesPerFrame;
    pAudioData->mBuffers[0].mData = pAudioSampleBuffer;
    
    UInt32 numberOfPacketsToRead = (UInt32) m_pBuffer_n;
    status = ExtAudioFileRead (fileRef,
                               &numberOfPacketsToRead,
                               pAudioData);
    
    // Kill the AudioBufferList
    if(pAudioData != NULL) {
        delete pAudioData;
        pAudioData = NULL;
    }
    
    // Convert all the samples to float and put into the other buffer
    m_pBuffer = (float *)malloc(sizeof(float) * m_pBuffer_n);
    for(int i = 0; i < m_pBuffer_n; i++) {
        if(pAudioSampleBuffer[i] >= 0)
            m_pBuffer[i] = pAudioSampleBuffer[i] / 32767.0f;
        else
            m_pBuffer[i] = pAudioSampleBuffer[i] / 32768.0f;
    }
    
    // Kill the buffer
    free(pAudioSampleBuffer);
    pAudioSampleBuffer = NULL;
    
Error:
    return r;
}


/*************************************/
// SampleNode
/*************************************/

SampleNode::SampleNode(char *pszFilenamePath) :
m_pSampleBuffer(NULL),
m_fPlaying(FALSE)
{
    SetChannelCount(1, CONN_OUT);
    m_pSampleBuffer = new SampleBuffer(pszFilenamePath);
}

RESULT SampleNode::SaveToFile(char *pszFilepath, bool fOverwrite) {
    RESULT r = R_SUCCEED;
    
    CRM(m_pSampleBuffer->SaveToFile(pszFilepath, fOverwrite), "SaveToFile: Buffer failed to save filepath %s", pszFilepath);
    
Error:
    return r;
}

SampleNode::~SampleNode() {
    if(m_pSampleBuffer != NULL) {
        delete m_pSampleBuffer;
        m_pSampleBuffer = NULL;
    }
}

RESULT SampleNode::Stop() {
    m_pSampleBuffer->ResetSampleCounter();
    m_fPlaying = false;
    return R_SUCCESS;
}

// Resume doesn't reset the counter
RESULT SampleNode::Resume() {
    m_fPlaying = true;
    return R_SUCCESS;
}

RESULT SampleNode::Trigger() {
    m_pSampleBuffer->ResetSampleCounter();
    m_fPlaying = true;
    return R_SUCCESS;
}

float SampleNode::GetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;
    
    if(m_fPlaying) {
        retVal = m_pSampleBuffer->GetNextSample(timestamp);
        
        if(m_pSampleBuffer->SampleDone()) {
            m_fPlaying = false;
            m_pSampleBuffer->ResetSampleCounter();
        }
    }
    
    return retVal;
}

SampleBuffer* SampleNode::GetSampleBuffer() {
    return m_pSampleBuffer;
}

float SampleNode::GetLength() {
    return m_pSampleBuffer->GetSampleBufferLengthMS();
}

RESULT SampleNode::SetStart(float msStart) {
    return m_pSampleBuffer->SetStart(msStart);
}

RESULT SampleNode::SetEnd(float msEnd) {
    return m_pSampleBuffer->SetEnd(msEnd);
}

RESULT SampleNode::SetTime(float msTime) {
    return m_pSampleBuffer->SetTime(msTime);
}

