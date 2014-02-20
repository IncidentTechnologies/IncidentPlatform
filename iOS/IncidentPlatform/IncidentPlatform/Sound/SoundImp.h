#pragma once

// This is the abstract class implementation of Sound Implementation
// for use with the SoundImpFactory and SoundObj

typedef enum SoundPlayingState
{
    SOUND_NOT_PLAYING = 0,
    SOUND_PLAYING = 1,
    SOUND_UNDETERMINED
} SOUND_PLAYING_STATE;

#define SOUND_STREAMING_BUFFER_DIVISIONS 4

class SoundImp
{
public:
    SoundImp(bool fStreaming = false) :
      m_pSoundObj(NULL),
      m_fStreaming(fStreaming),
      m_CurrentWritePosition(0),
      m_LastWritePosition(0)
    {/*empty stub*/}

    ~SoundImp(){/*emptystub*/}

public:

    virtual RESULT StreamBufferToImp(void *Buffer, int BufferSize, int offset) = 0;
    virtual RESULT StreamHungry(int &offset) = 0;   // This will return an offset into which to load the next buffer

    virtual RESULT PassBufferToImp(void*, int) = 0;
    virtual RESULT PlayBuffer(long Offset, bool Loop_f) = 0;
    virtual RESULT StopPlayback() = 0;
    virtual RESULT Release() = 0;
    virtual RESULT Initialize(SoundObj *pSoundObj) = 0;   

    /* FIX: Should get rid of SoundObj inside of the Implementation
     * The implementation and the data should be completely separate
     * this way the data can be manipulated separately */
    SoundObj *GetSoundObj(){ return m_pSoundObj; }
    RESULT SetSoundObj(SoundObj *pSoundObj){ m_pSoundObj = pSoundObj; return R_OK; }

public:
    // Pointer to the sound object
    SoundObj *m_pSoundObj;

public:
    virtual RESULT GetCurrentPlayPosition(long &Offset) = 0;
    virtual RESULT SetCurrentPlayPosition(long Offset) = 0;
    virtual RESULT GetCurrentWritePosition(long &Offset) = 0;
    virtual RESULT SetCurrentWritePosition(long Offset) = 0;
    virtual RESULT GetFrequency(long &Frequency) = 0;
    virtual RESULT SetFrequency(long Frequency) = 0;
    virtual RESULT GetPan(long &Pan) = 0;
    virtual RESULT SetPan(long Pan) = 0;
    virtual RESULT GetVolume(long &Volume) = 0;
    virtual RESULT SetVolume(long Volume) = 0;

public:
    COMPRESSION_CODE GetCompressionCode(){ return m_CompressionCode; }
    RESULT SetCompressionCode(COMPRESSION_CODE cd){ m_CompressionCode = cd; return R_OK; }
    int GetChannels(){ return m_Channels; }
    RESULT SetChannels(int c){ m_Channels = c; return R_OK; }
    int GetSampleRate(){ return m_SampleRate; }
    RESULT SetSampleRate(int s){ m_SampleRate = s; return R_OK; }
    long GetAvgBytesPerSecond(){ return m_AvgBytesPerSecond; }
    RESULT SetAvgBytesPerSecond(long a){ m_AvgBytesPerSecond = a; return R_OK; }
    int GetBlockAlign(){ return m_BlockAlign; }
    RESULT SetBlockAlign(int b){ m_BlockAlign = b; return R_OK; }
    int GetBitsPerSample(){ return m_BitsPerSample; }
    RESULT SetBitsPerSample(int bps){ m_BitsPerSample = bps; return R_OK; }
    long GetSamples(){ return m_Samples; }
    RESULT SetSamples( long s ){ m_Samples = s; return R_OK; }

    virtual SOUND_PLAYING_STATE SoundPlaying() = 0;
    
    //RESULT SetSoundPlaying(bool playing){ m_SoundPlaying_f = playing; return R_OK; }

    bool GetStreaming(){ return m_fStreaming; }
    RESULT SetStreaming(bool fStreaming){ m_fStreaming = fStreaming; return R_OK; }
    
private:
    // Wave format description 
    COMPRESSION_CODE m_CompressionCode;
    int m_Channels;
    int m_SampleRate;
    long m_AvgBytesPerSecond;
    int m_BlockAlign;
    int m_BitsPerSample;
    long m_Samples;

    // This should be tracked inside the actual
    // implementation
    //bool m_SoundPlaying_f;

public:
    // Streaming
    bool m_fStreaming;
    long m_CurrentWritePosition;
    long m_LastWritePosition;
};