#pragma once 

// Sound Wave is an encapsulation of opening a windows formatted WAVE file from the
// file system

#include "SoundObj.h"

// Temporary add a sound implementation here
// in reality the design will automatically assign
// one to the sound when it is created
#include "SoundImpFactory.h"

#include "Console.h"

class SoundBuffer;	// forward decl

/*typedef struct WaveFile
{
    
} WAVEFILE;*/

typedef enum
{
    CHUNK_ID_RIFF   = 0x52494646,   // Riff
    CHUNK_ID_FMT    = 0x666D7420,   // Format
    CHUNK_ID_DATA   = 0x64617461,   // Data
    CHUNK_ID_FACT   = 0x66616374,   // Fact
    CHUNK_ID_SLNT   = 0x736C6E74,   // Silent Chunk
    CHUNK_ID_CUE    = 0x63756520,   // Cue
    CHUNK_ID_PLST   = 0x736C6E74,   // Playlist
    CHUNK_ID_LIST   = 0x6C696E74,   // List
    CHUNK_ID_LBL    = 0x6C61626C,   // Label
    CHUNK_ID_NOTE   = 0x6E6F7465,   // Note
    CHUNK_ID_LTXT   = 0x6C747874,   // Labelled Text Chunk / Instrument
    CHUNK_ID_SMPL   = 0x736D706C    // Sample
} CHUNK_ID;

typedef enum WAVECOMPRESSIONCODE
{
    WAVE_COMPRESSION_CODE_UNKNOWN        =   0x0000,
    WAVE_COMPRESSION_CODE_PCM            =   0x0001,
    WAVE_COMPRESSION_CODE_MSFT_ADPCM     =   0x0002,
    WAVE_COMPRESSION_CODE_MPEG           =   0x0080,
    WAVE_COMPRESSiON_CODE_EXPERIMENTAL   =   0xFFFF
} WAVE_COMPRESSION_CODE;

typedef void* ChunkData;

struct Chunk
{
    // Header
    CHUNK_ID ID;
    int DataSize;
    // Data
    ChunkData Data;
};

struct RiffChunkData
{
    // Type Specific Data
    int Type;
    // While this is not part of the actual file format
    // we can stuff all of the chunks of the file into
    // a list here for easy access
    list<Chunk*> *ChunkList;
};

struct FormatChunkData
{
    // Type Specific Data
    short CompressionCode;		//2
    short Channels;				//4
    int SampleRate;				//8
    int AvgBytesPerSecond;		//12
    short BlockAlign;			//14
    short SigBitsPerSample;		//16
    short ExtraBytes;			//18

    // Buffer Data 
    void *ExtraBuffer;			//22
};

struct FactChunkData
{
    // Fact Data Dependent on Format
    void *Buffer;
};

struct DataChunkData
{
    // No type specific data
    // Buffer Data
    void *Buffer;
};


class SoundWave :
    public SoundObj
{
public:
    SoundWave(const char* pszFilename, SoundImpFactory *pSIF);

	SoundWave(SoundBuffer *pSoundBuffer, SoundImpFactory *pSIF);

    ~SoundWave();

    RESULT OpenWaveFile();
	RESULT CreateFromBuffer(SoundBuffer *pSoundBuffer);
	RESULT SaveToFile(char *pszFilename, bool fOverwrite);

    FormatChunkData* GetFormatChunk();

    void Print();

    // Get Format Information    
    COMPRESSION_CODE GetCompressionCode();
    int GetChannels();
    int GetSampleRate();
    long GetAvgBytesPerSecond();
    int GetBlockAlign();
    int GetBitsPerSample();
    long GetSampleCount();

	RESULT GetSamples(long &SampleCount, void * &n_pVSTBuffer)
	{
		return R_UNSUPPORTED;
	}

    // Two Play Modes for different purposes
    RESULT PlayData(long Offset);
    RESULT StreamData(long Offset);
    RESULT DestroyWaveFile();

    // Get the current sound status
    RESULT GetSoundStatus(SoundStatus* &n_pSS);
    bool SoundPlaying(){ return m_pSoundImp->SoundPlaying(); }

private:
    char *m_pszFilename;
    wchar_t *m_pwszPath;
    char *m_pszFilePath;
    FILE *fileWave;    

public:
    Chunk* m_pRiffChunk;

public:
    // Associate a sound implementation for the time being
    // this way m_sids will be called to play the sound
    // this is a tricky part in regards to performance
    SoundImp *m_pSoundImp;

    // This sound imp factory is responsible for creating the DirectSoundImplementation
    SoundImpFactory *m_SoundImpFactory;
};

//RESULT TestSoundWave();
RESULT TestSoundWave(Console *pc, char *pszFilename);