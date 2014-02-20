#pragma once

// This is a monophonic stereo sawtooth synthesizer implementation
// it will fill up the buffer dynamically and send
// data to the implementation as required.

#include "SoundObj.h"
#include "SoundImpFactory.h"

// Should make these adjustable and dynamic as well
#define SOUND_SAWTOOTH_SAMPLES 1024
#define SOUND_SAWTOOTH_BITSPERSAMPLE 16
#define SOUND_SAWTOOTH_CHANNELS 2
#define SOUND_SAWTOOTH_SAMPLESPERSEC 44100
#define SOUND_SAWTOOTH_WAVE_FORMAT WAVE_FORMAT_PCM
#define SOUND_SAWTOOTH_AVG_BYTES_PER_SEC SOUND_SAWTOOTH_SAMPLESPERSEC * 4
#define SOUND_SAWTOOTH_BLOCK_ALIGN 4

class SoundSawtooth :
    public SoundObj
{
public:
    SoundSawtooth(SoundImpFactory* pSIF) :
      m_NoteOn_f(false),
      m_SoundImpFactory(pSIF),
      m_BufferSize(SOUND_SAWTOOTH_SAMPLES),
      m_NoteFreq(0)
    {
        RESULT r = R_OK;

        // Set up the implementation
        if(m_SoundImpFactory)
        {
            m_pSoundImp = m_SoundImpFactory->MakeSoundImp();
            //m_pSoundImp->Initialize(this);
        }
        else
        {
            printf("WARNING: SoundSawtooth not complete since factory is NULL\n");
        }

    }

    ~SoundSawtooth()
    {
        // Kill the Implementation
        if (m_pSoundImp != NULL)
        {
            m_pSoundImp->Release();
            delete m_pSoundImp;
        }
    }

    // These Interfaces are defined so that a generic implementation
    // can get this information out of the data.
    COMPRESSION_CODE GetCompressionCode(){ return COMPRESSION_CODE_PCM; }
    int GetChannels(){ return SOUND_SAWTOOTH_CHANNELS; }
    int GetSampleRate(){ return SOUND_SAWTOOTH_SAMPLESPERSEC; }
    long GetAvgBytesPerSecond(){ return SOUND_SAWTOOTH_AVG_BYTES_PER_SEC; }
    int GetBlockAlign(){ return SOUND_SAWTOOTH_BLOCK_ALIGN; }
    int GetBitsPerSample(){ return SOUND_SAWTOOTH_BITSPERSAMPLE; }

    // Since this is a synthesizer the samples tend to change a lot
    long GetSampleCount()
    { 
        return m_BufferSize; 
    }        

	RESULT GetSamples(long &SampleCount, void * &n_pVSTBuffer)
	{
		return R_UNSUPPORTED;
	}

    void Print()
    { 
        printf("Sound Sawtooth Synthesizer Object\n"); 
    }

    RESULT GetSoundStatus(SoundStatus* &n_pSS)
    {        
        RESULT r = R_OK;

        n_pSS = new SoundStatus;
        memset(n_pSS, 0, sizeof(SoundStatus));

        n_pSS->Playing_f = m_pSoundImp->SoundPlaying(); 
        CRM(m_pSoundImp->GetCurrentPlayPosition(n_pSS->PlayOffset), "SoundImp::GetCurrentPlayPosition");
        
        /*FIX*/n_pSS->WriteOffset = 0;  // don't know how to implement this yet
        
        CRM(m_pSoundImp->GetVolume(n_pSS->Volume), "SoundImp::GetVolume");    
        CRM(m_pSoundImp->GetPan(n_pSS->Pan), "SoundImp::GetPan");

Error:
        return r;        
    }

    // This function is used a lot so avoid memory 
    // when using it
    bool SoundPlaying()
    {
        return m_pSoundImp->SoundPlaying();
    }

    RESULT PlayNote(float frequency)
    {
        RESULT r = R_OK;

        if(m_NoteOn_f)
        {
            KillNote();                   
        }

        m_NoteFreq = frequency;

        // First create a buffer and fill with the correct frequency
        float TimeStep = 1.0f / GetSampleRate();
        float Period = 1.0f / frequency;    
        
        m_BufferSize = (int)(Period / TimeStep);
        //m_BufferSize = SOUND_SAWTOOTH_SAMPLES;

        short *pSawtoothBuffer = new short[m_BufferSize];           
            
        for(int i = 0; i < m_BufferSize; i++)
        {
            pSawtoothBuffer[i] = (0x7FFF / (i + 1)); //(short)((Period / (float)((i % Steps) * TimeStep)) * (float)(0x7FFF));
        }

        m_pSoundImp->Initialize(this);      
        m_pSoundImp->PassBufferToImp((void*)pSawtoothBuffer, m_BufferSize);
        m_pSoundImp->PlayBuffer(0, true);
        m_NoteOn_f = true;

Error:
        return r;
    }

    // Will pitch bend the frequency a certain amount (percentage)
    RESULT BendNotePitch(float amount)
    {
        RESULT r = R_OK;

        if(!m_NoteOn_f)
        {
            return r;
        }
        
        SoundImpDirectSound* pDSoundImp = reinterpret_cast<SoundImpDirectSound*>(m_pSoundImp);
        DWORD Freq = (int)((float)(GetSampleRate()) * (1.0f + amount));
        pDSoundImp->m_pDSoundBuffer->SetFrequency(Freq);

Error:
        return r;
    }

    // Since this is a monophonic synth we only worry about
    // killing the note in general
    RESULT KillNote(float frequency)
    {
        RESULT r = R_OK;

        // Only kill the note for the specific frequency
        if(m_NoteOn_f && m_NoteFreq == frequency)
        {        
            m_pSoundImp->StopPlayback();
            m_NoteOn_f = false;
        }

Error:
        return r;
    }

    // Kills note indiscriminantly 
    RESULT KillNote()
    {
        RESULT r = R_OK;

        // Only kill the note for the specific frequency
        if(m_NoteOn_f)
        {        
            m_pSoundImp->StopPlayback();
            m_NoteOn_f = false;
        }

Error:
        return r;
    }

private:
    bool m_NoteOn_f;
    float m_NoteFreq;

    int m_BufferSize;

    SoundImp *m_pSoundImp;
    SoundImpFactory *m_SoundImpFactory;
};