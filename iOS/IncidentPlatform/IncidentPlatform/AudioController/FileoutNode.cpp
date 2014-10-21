//
//  FileoutNode.cpp
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "FileoutNode.h"
#include "CAXException.h"

FileoutNode::FileoutNode(char *pszFilepath, bool fOverwrite) :
    m_refFileout(NULL),
    m_timestamp(0)
{
    SetChannelCount(1, CONN_OUT);
    SetChannelCount(1, CONN_IN);
    
    CFStringRef cfstrPath = CFStringCreateWithCStringNoCopy(NULL, pszFilepath, kCFStringEncodingMacRoman, NULL);
    CFURLRef outputFileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                                                           cfstrPath,
                                                           kCFURLPOSIXPathStyle,
                                                           false);
    
    // Create the file
    AudioStreamBasicDescription LocalDataFormat = GetClientFormatDescription(false);
    
    AudioFileTypeID fileType = kAudioFileWAVEType;
    AudioStreamBasicDescription OutputDataFormat;
    memset(&OutputDataFormat, 0, sizeof(AudioStreamBasicDescription));
    
    OutputDataFormat.mSampleRate         = 44100.0;
    OutputDataFormat.mFormatID           = kAudioFormatLinearPCM;
    OutputDataFormat.mFormatFlags        = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    OutputDataFormat.mChannelsPerFrame = 1;
    OutputDataFormat.mBytesPerPacket = 2; //0
    OutputDataFormat.mBytesPerFrame = 2; //0
    OutputDataFormat.mFramesPerPacket = 1; //1024
    OutputDataFormat.mBitsPerChannel = 16; //0;
    OutputDataFormat.mReserved = 0;
    
    
    UInt32 flags = (fOverwrite) ? kAudioFileFlags_EraseFile : 0;
    OSStatus status = ExtAudioFileCreateWithURL(outputFileURL,
                                                fileType,
                                                &OutputDataFormat,
                                                NULL,
                                                flags,
                                                &m_refFileout);
    if(status) {
        DEBUG_LINEOUT("FileoutNode: ExtAudioFileCreate failed: %s", CAX4CCString(status).get());
    }
    
    // Specify Codec
    UInt32 codec = kAppleSoftwareAudioCodecManufacturer;
    status = ExtAudioFileSetProperty(m_refFileout,
                                     kExtAudioFileProperty_CodecManufacturer,
                                     sizeof(codec),
                                     &codec);
    
    if(status)
        DEBUG_LINEOUT("FileoutNode: ExtAudioFileSetProperty:kExtAudioFileProperty_CodecManufacturer failed: %s", CAX4CCString(status).get());
    
    
    // Tell the ExtAudioFile API what format we'll be sending samples in
    //LocalDataFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked | kAudioFormatFlagsNativeEndian;
    //LocalDataFormat.mChannelsPerFrame = 2;
    
    status = ExtAudioFileSetProperty(m_refFileout,
                                     kExtAudioFileProperty_ClientDataFormat,
                                     sizeof(AudioStreamBasicDescription),
                                     &LocalDataFormat);
    
    if (status) {
        DEBUG_LINEOUT("FileoutNode: ExtAudioFileSetProperty failed: %s", CAX4CCString(status).get());
    }
    
    
    
}

RESULT FileoutNode::SaveSamples(unsigned long samples_n) {
    RESULT r = R_SUCCESS;
    
    // Set up local format
    AudioStreamBasicDescription LocalDataFormat = GetClientFormatDescription(false);
    
    // Create the audio buffer and copy
    AudioSampleType *pAudioSampleBuffer;
    pAudioSampleBuffer = (AudioSampleType*)malloc(sizeof(AudioSampleType) * samples_n);
    
    for(int i = 0; i < samples_n; i++) {
        pAudioSampleBuffer[i] = GetNextSample(m_timestamp) * 32767.0f;
        m_timestamp++;
    }
    
    // Set up the audio bufer list
    AudioBufferList *pAudioData = new AudioBufferList();
    pAudioData->mNumberBuffers = 1;
    pAudioData->mBuffers[0].mNumberChannels = LocalDataFormat.mChannelsPerFrame;
    pAudioData->mBuffers[0].mDataByteSize = (UInt32)(samples_n * LocalDataFormat.mBytesPerFrame);
    pAudioData->mBuffers[0].mData = pAudioSampleBuffer;
    
    UInt32 numberOfFramesToWrite = (UInt32) samples_n;
    OSStatus status = ExtAudioFileWrite (m_refFileout,
                                numberOfFramesToWrite,
                                pAudioData);
    
    if(status)
        DEBUG_LINEOUT("FileoutNode: SaveSamples: ExtAudioFileWrite failed: %s", CAX4CCString(status).get());
    
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


FileoutNode::~FileoutNode() {
    // Close the file
    OSStatus status = ExtAudioFileDispose(m_refFileout);

    if(status)
        DEBUG_LINEOUT("FileoutNode:~: ExtAudioFileDispose failed: %s", CAX4CCString(status).get());
    
}

AudioStreamBasicDescription FileoutNode::GetClientFormatDescription(bool fStereo) {
    AudioStreamBasicDescription ClientDataFormat;
    memset(&ClientDataFormat, 0, sizeof(AudioStreamBasicDescription));
    
    size_t bytesPerSample = sizeof (AudioSampleType);
    
    ClientDataFormat.mFormatID          = kAudioFormatLinearPCM;
    ClientDataFormat.mFormatFlags       = kAudioFormatFlagIsSignedInteger;
    ClientDataFormat.mChannelsPerFrame  = 1;                    // 1 indicates mono
    ClientDataFormat.mBytesPerFrame     = bytesPerSample;       //ClientDataFormat.mChannelsPerFrame * sizeof(float);
    ClientDataFormat.mFramesPerPacket   = 1;
    ClientDataFormat.mBytesPerPacket    = bytesPerSample;       //ClientDataFormat.mBytesPerFrame * ClientDataFormat.mFramesPerPacket;
    ClientDataFormat.mBitsPerChannel    = 8 * bytesPerSample;   //8 * sizeof(float);
    ClientDataFormat.mSampleRate        = m_SampleRate;
    
    return ClientDataFormat;
}