#pragma once

/* ***********************************************************************************
   This object is meant as an arbitrary buffer sound object that can be filled with 
   data and manipulated.  Potentially this could be used to derive the other sound
   objects.  We can reconsider the design of the sound engine based on how this works
*  ***********************************************************************************/

#include "SoundObj.h"
#include "SoundImpFactory.h"
#include "SoundCmn.h"

#include "SoundWave.h"	

#include <complex>


#define SOUND_BUFFER_WAVE_FORMAT COMPRESSION_CODE_PCM_FLOAT
#define SOUND_BUFFER_CHANNELS 1
#define SOUND_BUFFER_BITS_PER_SAMPLE (sizeof(float) * 8);
#define SOUND_BUFFER_BYTES_PER_SAMPLE (sizeof(float));
#define SOUND_BUFFER_BLOCK_ALIGN 4


typedef struct ComplexFloatBuffer
{
    std::complex<float> *buffer;
    long buffer_n;
};

class SoundBuffer :
    public SoundObj
{
public:
    // Time is in seconds
    // sample rate is in Hertz
    SoundBuffer(float TimeLength, int SampleRate, SoundImpFactory *pSIF);
    ~SoundBuffer();

    void Print() { printf("SoundObject: SoundBuffer"); }

    // Get Format Information    
    COMPRESSION_CODE GetCompressionCode(){ return SOUND_BUFFER_WAVE_FORMAT; }
    
    int GetChannels(){ return SOUND_BUFFER_CHANNELS; }
    int GetSampleRate(){ return m_SampleRate; }
    
    long GetAvgBytesPerSecond()
    { 
        long BitsPerSec = m_SampleRate * SOUND_BUFFER_BITS_PER_SAMPLE;
        long BytesPerSec = BitsPerSec >> 3;
        return BytesPerSec; 
    }

    int GetBlockAlign(){ return SOUND_BUFFER_BLOCK_ALIGN; }
    int GetBitsPerSample(){ return SOUND_BUFFER_BITS_PER_SAMPLE; }
    long GetSampleCount()
    { 
        return m_floatBuffer_n; 
    }

	RESULT GetSamples(long &SampleCount, void * &n_pVSTBuffer)
	{
		return R_UNSUPPORTED;
	}

    // Two Play Modes for different purposes
    //RESULT PlayData(long Offset);
    //RESULT StreamData(long Offset);

    // Get the current sound status
    //RESULT GetSoundStatus(SoundStatus* &n_pSS);

    // Play data will send the Wave File data to the DirectSound object
    // and will lump together all the different chunks into one memory location.
    RESULT PlayData(long Offset);

    // This will simply reset the buffer to whatever is passed to it
    // this will not change the sampling rate but it will recalculate the time length
    RESULT PassBuffer(float* &d_pBuffer, long &d_pBuffer_n);

	//RESULT SaveToMp3File(char *pszFilepath, int bitrate);

	RESULT GetSoundWave(SoundWave* &n_soundWave);

    // Will return the sound status 
    // allocates the memory for the Sound Status 
    // structure
    RESULT GetSoundStatus(SoundStatus* &n_pSS);

    RESULT SaveBufferToFile(char *pszFilename);

    RESULT rFFT();

private:
    ComplexFloatBuffer rFFT(ComplexFloatBuffer fb, std::complex<float>g);

public:
    bool SoundPlaying()
    { 
        SOUND_PLAYING_STATE sps = m_pSoundImp->SoundPlaying();
        return (sps == SOUND_PLAYING) ? true : false;
    } 

	float *GetBuffer(){ return m_floatBuffer; }
	long GetBufferLength(){ return m_floatBuffer_n; }
	long GetBufferSize(){ return m_floatBuffer_n * SOUND_BUFFER_BYTES_PER_SAMPLE; }

private:
    float *m_floatBuffer;
    long m_floatBuffer_n;

public:
    float m_TimeLength;
    int m_SampleRate;

    // Associate a sound implementation for the time being
    // this way m_sids will be called to play the sound
    // this is a tricky part in regards to performance
    SoundImp *m_pSoundImp;

    // This sound imp factory is responsible for creating the DirectSoundImplementation
    SoundImpFactory *m_SoundImpFactory;
};