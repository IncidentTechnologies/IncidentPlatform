//
//  SampleNode.cpp
//  IncidentPlatform
//
//  Created by Idan Beck on 2/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "SampleNode.h"


SampleBuffer::SampleBuffer(char *pszFilenamePath) :
    m_pBuffer_c(0),
    m_pBuffer(NULL),
    m_pBuffer_n(0),
    m_SampleRate(DEFAULT_SAMPLE_RATE)
{
    LoadSampleBufferFromPath(pszFilenamePath);
}

bool SampleBuffer::SampleDone() {
    return (m_pBuffer_c >= m_pBuffer_n);
}

RESULT SampleBuffer::ResetSampleCounter() {
    m_pBuffer_c = 0;
    return R_SUCCESS;
}

float SampleBuffer::GetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;
    
    if(m_pBuffer != NULL && m_pBuffer_n > 0 && m_pBuffer_c < m_pBuffer_n) {
        retVal = m_pBuffer[m_pBuffer_c];
        m_pBuffer_c++;
    }
    
    return retVal;
}

// TODO: Add Stereo
AudioStreamBasicDescription SampleBuffer::GetClientFormatDescription(bool fStereo) {
    AudioStreamBasicDescription ClientDataFormat;
    memset(&ClientDataFormat, 0, sizeof(AudioStreamBasicDescription));
    
    size_t bytesPerSample = sizeof (AudioSampleType);
    
    ClientDataFormat.mFormatID          = kAudioFormatLinearPCM;
    ClientDataFormat.mFormatFlags       = kLinearPCMFormatFlagIsSignedInteger;
    ClientDataFormat.mChannelsPerFrame  = 1;                    // 1 indicates mono
    ClientDataFormat.mBytesPerFrame     = bytesPerSample;       //ClientDataFormat.mChannelsPerFrame * sizeof(float);
    ClientDataFormat.mFramesPerPacket   = 1;
    ClientDataFormat.mBytesPerPacket    = bytesPerSample;       //ClientDataFormat.mBytesPerFrame * ClientDataFormat.mFramesPerPacket;
    ClientDataFormat.mBitsPerChannel    = 8 * bytesPerSample;   //8 * sizeof(float);
    ClientDataFormat.mSampleRate        = m_SampleRate;
    
    return ClientDataFormat;
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
    CFRelease(cfstrPath);
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
    AudioSampleType *pAudioSampleBuffer;
    pAudioSampleBuffer = (AudioSampleType*)malloc(sizeof(AudioSampleType) * m_pBuffer_n);
    
    // Set the client format description
    AudioStreamBasicDescription ClientDataFormat = GetClientFormatDescription(false);
    status =    ExtAudioFileSetProperty (fileRef,
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

SampleNode::SampleNode(char *pszFilenamePath) :
    m_pSampleBuffer(NULL),
    m_fPlaying(FALSE)
{
    SetChannelCount(1, CONN_OUT);
    m_pSampleBuffer = new SampleBuffer(pszFilenamePath);
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

