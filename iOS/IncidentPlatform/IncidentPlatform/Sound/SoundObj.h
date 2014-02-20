#pragma once

// Sound Object is the generic interface for a sound object
// with a specific buffer.  The generic interface, in fact,
// owns the buffer and the child classes can only access the buffer 
// through it.

#include "cmnPriv.h"
#include "valid.h"

class SoundImp;

typedef enum SOUNDOBJECTTYPE
{
    SOUND_OBJECT_ONESHOT,
    SOUND_OBJECT_LOOP,
    SOUND_OBJECT_REPEAT
} SOUND_OBJECT_TYPE;

typedef enum COMPRESSIONCODE
{
    COMPRESSION_CODE_UNKNOWN        =   0x0000,
    COMPRESSION_CODE_PCM            =   0x0001,
    COMPRESSION_CODE_MSFT_ADPCM     =   0x0002,
    COMPRESSION_CODE_MPEG           =   0x0080,
    COMPRESSION_CODE_PCM_FLOAT      =   0x0100,
    COMPRESSiON_CODE_EXPERIMENTAL   =   0xFFFF
} COMPRESSION_CODE;

// Sound Status is meant to
// indicate the current status
// of the sound (play back and otherwise)
struct SoundStatus
{
    bool Playing_f;
    long PlayOffset;
    long WriteOffset;
    long Volume;
    long Pan;
};

class SoundObj :
    public valid
{
public:
    SoundObj() :
      m_pszName(NULL),
      m_Duration_ms(0.0f),
      m_Amplitude(1.0f)
    {
        Validate();  
    }

    SoundObj(const char* pszName, float Duration_ms)
    {
        m_pszName = new char[strlen(pszName)];
        strcpy(m_pszName, pszName);
        m_Duration_ms = Duration_ms;
        m_Amplitude = 1.0f;

        Validate(); 
    }

    ~SoundObj()
    {/*empty stub*/}

    char *GetName(){ return m_pszName; }
    float GetDuration(){ return m_Duration_ms; }
    float GetAmplitude(){ return m_Amplitude; }
    float SetAmplitude(float Amplitude){ m_Amplitude = Amplitude; return m_Amplitude; }

    // These Interfaces are defined so that a generic implementation
    // can get this information out of the data.
    virtual COMPRESSION_CODE GetCompressionCode() = 0;
    virtual int GetChannels() = 0;
    virtual int GetSampleRate() = 0;
    virtual long GetAvgBytesPerSecond() = 0;
    virtual int GetBlockAlign() = 0;
    virtual int GetBitsPerSample() = 0;

    // This will return the total number of samples in the sound
    virtual long GetSampleCount() = 0;              
    virtual void Print(){ printf("Sound Object Generic Interface\n"); }

	virtual RESULT GetSamples(long &SampleCount, void * &n_pVSTBuffer) = 0;

    virtual RESULT GetSoundStatus(SoundStatus* &n_pSS) = 0;
    
    // This function is used a lot so avoid memory 
    // when using it
    virtual bool SoundPlaying() = 0;

private:
    char *m_pszName;
    float m_Duration_ms;
    float m_Amplitude;

    //SoundImp* m_pSoundImp;

    SOUND_OBJECT_TYPE m_SoundObjectType;
};