#include "SoundWave.h"
#include "SoundBuffer.h"

// Copy over backwards
// assumes both buffers are already allocated
RESULT CopyLittleEndian(void *dest, void *src, int len)
{
    RESULT r = R_OK;

    CNR(dest);
    CNR(src);

    char *dest_cur = (char*)dest;
    char *src_cur = (char*)src;

    for(int i = 0; i < len; i++)
    {   
        //dest_cur = dest + (len - i - 1);
        dest_cur[len - i - 1] = src_cur[i]; 
    }
    
Error:
    return r;
}

// fwrite backwards
RESULT fwriteBigEndian(void *src, long size, FILE *pFile)
{
	RESULT r = R_OK;

	for(int i = 0; i < size; i++)
		fwrite((void*)((long)src + (size - i - 1)), 1, 1, pFile);

Error:
	return r;
}

RESULT ReadChunk(FILE *file, Chunk * &nm_pChunk)
{
    RESULT r = R_OK;    
    BYTE TempByteBuffer[4];
    int BytesRead = 0;

    nm_pChunk = (Chunk*)malloc(sizeof(Chunk));;
    memset(nm_pChunk, 0, sizeof(Chunk));
    
    BytesRead = fread(TempByteBuffer, sizeof(BYTE), 4, file);
    if(BytesRead != 4)
    {       
        if(feof(file))        
        {
            printf("Reached end of file!\n");  
            // dealocate the memory and set it to NULL
            delete nm_pChunk;
            nm_pChunk = NULL;
            goto Error;
        }
        else
            printf("Error occured in reading in wave file data (chunk header ID)!\n");        
    }
    else
    {
        // Little Endian
        CopyLittleEndian(&nm_pChunk->ID, TempByteBuffer, 4*sizeof(BYTE));
    }

    if(fread(TempByteBuffer, sizeof(BYTE), 4, file) != 4)
    {
        printf("Error occurred in reading in wave file data (chunk header data size)!\n");
    }
    else
    {
        memcpy(&nm_pChunk->DataSize, TempByteBuffer, 4*sizeof(BYTE));
        //CopyLittleEndian(&chunk.DataSize, TempByteBuffer, 4*sizeof(BYTE));
    }

    switch(nm_pChunk->ID)
    {
        case CHUNK_ID_RIFF:
        {            
            RiffChunkData *riff = new RiffChunkData;
            memset(riff, 0, sizeof(RiffChunkData));

            if(fread(TempByteBuffer, sizeof(BYTE), 4, file) != 4)
            {
                printf("Error occurred in reading in wave file data (riff type data)!\n");
            }
            else
            {
                // Little Endian
                CopyLittleEndian(&(riff->Type), TempByteBuffer, 4*sizeof(BYTE));
            }

            printf("riff chunk size:%d\n", nm_pChunk->DataSize);

            riff->ChunkList = NULL;

            nm_pChunk->Data = riff;
        } break;

        case CHUNK_ID_FMT:
        {
            FormatChunkData *fmt = new FormatChunkData;
            memset(fmt, 0, sizeof(FormatChunkData));
            
            // Compression Code
            if(fread(&(fmt->CompressionCode), sizeof(BYTE), 2, file) != 2)            
                printf("Error occurred in reading in wave file data (fmt compression code)\n");
            // Channels
            if(fread(&(fmt->Channels), sizeof(BYTE), 2, file) != 2)            
                printf("Error occurred in reading in wave file data (fmt channels)\n");
            // Sample Rate
            if(fread(&(fmt->SampleRate), sizeof(BYTE), 4, file) != 4)            
                printf("Error occurred in reading in wave file data (fmt sample rate)\n");
            // Average Bytes Per Second
            if(fread(&(fmt->AvgBytesPerSecond), sizeof(BYTE), 4, file) != 4)            
                printf("Error occurred in reading in wave file data (fmt average bytes per second)\n");
            // Block Alignment
            if(fread(&(fmt->BlockAlign), sizeof(BYTE), 2, file) != 2)            
                printf("Error occurred in reading in wave file data (fmt block align)\n");
            // Significant Bites Per Sample
            if(fread(&(fmt->SigBitsPerSample), sizeof(BYTE), 2, file) != 2)            
                printf("Error occurred in reading in wave file data (fmt significant bits per sample)\n");
            // Check to see if we need the extra format field
            if(nm_pChunk->DataSize > 16)
            {
                // Extra Bytes in Format
                if(fread(&(fmt->ExtraBytes), sizeof(BYTE), 2, file) != 2)            
                    printf("Error occurred in reading in wave file data (fmt extra byte count)\n");  
                if(fmt->ExtraBytes == (nm_pChunk->DataSize - 16))
                {
                    // Extra Byte Buffer
                    fmt->ExtraBuffer = (void*)malloc(fmt->ExtraBytes * sizeof(BYTE));
                    if(fread(fmt->ExtraBuffer, sizeof(BYTE), fmt->ExtraBytes, file) != fmt->ExtraBytes)
                        printf("Error occurred in reading in wave file data (fmt extra buffer)\n");                
                }
                else
                {
                    // Chunk tells us that there is extra there
                    // but none is found
                    printf("File error: Expecting %d extra bits but only found: %d skipping...\n", (nm_pChunk->DataSize - 16), fmt->ExtraBytes);
                    fmt->ExtraBytes = 0;
                    fmt->ExtraBuffer = NULL;
                }
            }
            else
            {
                fmt->ExtraBytes = 0;
                fmt->ExtraBuffer = NULL;
            }

            printf("fmt chunk size:%d\n", nm_pChunk->DataSize);

            nm_pChunk->Data = fmt;
        } break;

        case CHUNK_ID_DATA:
        {
            DataChunkData *data = new DataChunkData;
            memset(data, 0, sizeof(DataChunkData));

            if(nm_pChunk->DataSize > 0)
            {
                data->Buffer = (void*)malloc(nm_pChunk->DataSize * sizeof(BYTE));
                // Read in the data chunk
                int read = fread(data->Buffer, sizeof(BYTE), nm_pChunk->DataSize, file);
                if(read != nm_pChunk->DataSize)
                {
                    printf("Error occurred in reading in wave file data (data chunk data)\n");
                    printf("read:%d expecting:%d\n", read, nm_pChunk->DataSize);
                }
            }
            else
            {
                // empty data chunk encountered!
                printf("Error: Empty data chunk encountered!\n");
                data->Buffer = NULL;
            }

            printf("data chunk size:%d\n", nm_pChunk->DataSize);
            nm_pChunk->Data = data;
        } break;

        case CHUNK_ID_FACT:
        {
            FactChunkData *fcd = new FactChunkData;
            memset(fcd, 0, sizeof(FactChunkData));

            if(nm_pChunk->DataSize > 0)
            {
                fcd->Buffer = (void*)malloc(nm_pChunk->DataSize * sizeof(BYTE));
                // Read in the data
                int read = fread(fcd->Buffer, sizeof(BYTE), nm_pChunk->DataSize, file);
                if(read != nm_pChunk->DataSize)
                {
                    printf("Error occurred in reading in wave file data (fact chunk data)\n");
                    printf("read:%d expecting:%d\n", read, nm_pChunk->DataSize);
                }
            }
            else
            {
                // empty fact data chunk encountered!
                printf("Error: Empty fact data chunk encountered!\n");
                fcd->Buffer = NULL;
            }

            printf("fact chunk size:%d\n", nm_pChunk->DataSize);
            nm_pChunk->Data = fcd;
        } break;


        default:
        {
            printf("Unknown Chunk ID Type: 0x%x size:%d\n", nm_pChunk->ID, nm_pChunk->DataSize);
            // Should forward in the file the size of the chunk
            // as to not lose our place!
            if(fseek(file, nm_pChunk->DataSize, SEEK_CUR) != 0)
            {
                printf("Error in fseek!\n");
            }

            nm_pChunk = NULL;
        } break;
    }

Error:
    return r;
}

RESULT SoundWave::CreateFromBuffer(SoundBuffer *pSoundBuffer)
{
	RESULT r = R_OK;

	// Set up the riff chunk
	// we will add the chunk size once all the chunks have been set up
	m_pRiffChunk = (Chunk*)malloc(sizeof(Chunk));;
	memset(m_pRiffChunk, 0, sizeof(Chunk));
	m_pRiffChunk->ID = CHUNK_ID_RIFF;
	m_pRiffChunk->Data = new RiffChunkData;
	m_pRiffChunk->DataSize = 0;
	memset(m_pRiffChunk->Data, 0, sizeof(RiffChunkData));
	reinterpret_cast<RiffChunkData*>(m_pRiffChunk->Data)->ChunkList = NULL;	// no chunk list in the riff chunk
	reinterpret_cast<RiffChunkData*>(m_pRiffChunk->Data)->Type = 0x57415645;

	// Set up the other chunks and attach them to the riff chunk
	list<Chunk*> *tempChunks = new list<Chunk*>();

	// Format Chunk
	Chunk *pFMTChunk = (Chunk *)malloc(sizeof(Chunk));
	memset(pFMTChunk, 0, sizeof(Chunk));
	pFMTChunk->ID = CHUNK_ID_FMT;

	FormatChunkData *pFMTChunkData = new FormatChunkData;
	pFMTChunkData->AvgBytesPerSecond = pSoundBuffer->GetAvgBytesPerSecond();
	pFMTChunkData->BlockAlign = pSoundBuffer->GetBlockAlign();
	//pFMTChunkData->BlockAlign = 2;
	pFMTChunkData->Channels = pSoundBuffer->GetChannels();
	//pFMTChunkData->CompressionCode = pSoundBuffer->GetCompressionCode();
	pFMTChunkData->CompressionCode = WAVE_COMPRESSION_CODE_PCM;
	pFMTChunkData->ExtraBuffer = NULL; 
	pFMTChunkData->ExtraBytes = 0;
	pFMTChunkData->SigBitsPerSample = pSoundBuffer->GetBitsPerSample();
	//pFMTChunkData->SigBitsPerSample = 16;
	pFMTChunkData->SampleRate = pSoundBuffer->GetSampleRate();

	pFMTChunk->Data = pFMTChunkData;
	pFMTChunk->DataSize = sizeof(FormatChunkData) - sizeof(void*) - sizeof(short);
	//pFMTChunk->DataSize = 16;

	tempChunks->Append(pFMTChunk);

	// add to RIFF chunk size
	m_pRiffChunk->DataSize += sizeof(FormatChunkData) + (sizeof(Chunk) - sizeof(ChunkData));

	// Data Chunk
	Chunk *pDataChunk = (Chunk *)malloc(sizeof(Chunk));
	memset(pDataChunk, 0, sizeof(Chunk));
	pDataChunk->ID = CHUNK_ID_DATA;

	DataChunkData *pDataChunkData = new DataChunkData;
	pDataChunkData->Buffer = malloc(pSoundBuffer->GetBufferSize());
	memcpy(pDataChunkData->Buffer, pSoundBuffer->GetBuffer(), pSoundBuffer->GetBufferSize());
	pDataChunk->Data = pDataChunkData;
	pDataChunk->DataSize = pSoundBuffer->GetBufferSize();
	
	m_pRiffChunk->DataSize += pSoundBuffer->GetBufferSize() + (sizeof(Chunk) - sizeof(ChunkData)) - 2;
	printf("final data size: %d\r\n", m_pRiffChunk->DataSize);

	tempChunks->Append(pDataChunk);

	// Fact Chunk
	Chunk *pFactChunk = (Chunk *)malloc(sizeof(Chunk));
	memset(pFactChunk, 0, sizeof(Chunk));
	pFactChunk->ID = CHUNK_ID_FACT;

	FactChunkData *pFactChunkData = new FactChunkData;
	pFactChunkData->Buffer = malloc(4);
	int tempSize = pSoundBuffer->GetBufferSize();
	memcpy(pFactChunkData->Buffer, &tempSize, 4);
		pFactChunk->DataSize = 4;
	pFactChunk->Data = pFactChunkData;
	tempChunks->Append(pFactChunk);

	reinterpret_cast<RiffChunkData*>(m_pRiffChunk->Data)->ChunkList = tempChunks;

Error:
	return r;
}

RESULT SoundWave::SaveToFile(char *pszFilename, bool fOverwrite)
{
	RESULT r = R_OK;

	FILE *pFile;
	// check if file already exists
	if(!fOverwrite)
		CBRM(!fopen(pszFilename, "r"), "File %s already exists!", pszFilename);

	pFile = fopen(pszFilename, "wb");

	// Now write to the file
	// the Riff chunk
	//fwrite(m_pRiffChunk, 1, 8, pFile); 
	fwriteBigEndian(&(m_pRiffChunk->ID), 4, pFile );
	//fwriteBigEndian(&(m_pRiffChunk->DataSize), 4, pFile);
	fwrite(&(m_pRiffChunk->DataSize), 1, 4, pFile);
	fwriteBigEndian(&(reinterpret_cast<RiffChunkData*>(m_pRiffChunk->Data)->Type), 4, pFile);	// the type

	// Get the other chunks
	Chunk *pFmtChunk  = NULL;
	Chunk *pDataChunk = NULL;
	Chunk *pFactChunk = NULL;
	list<Chunk*> *temp = reinterpret_cast<RiffChunkData*>(m_pRiffChunk->Data)->ChunkList;
	for(list<Chunk*>::iterator it = temp->First(); it != NULL; it++)
	{
		CHUNK_ID cid = reinterpret_cast<Chunk*>(*it)->ID;
		if(cid == CHUNK_ID_FMT)
			pFmtChunk = reinterpret_cast<Chunk*>(*it);
		else if(cid == CHUNK_ID_DATA)
			pDataChunk = reinterpret_cast<Chunk*>(*it);
		else if(cid == CHUNK_ID_FACT)
			pFactChunk = reinterpret_cast<Chunk*>(*it);
	}

	CNRM(pFmtChunk, "SoundWave:SaveToFile:No format chunk found");
	CNRM(pDataChunk, "SoundWave:SaveToFile:No data chunk found!");

	// FMT Chunk
	fwriteBigEndian(&pFmtChunk->ID, 4, pFile);
	//fwrite(pFmtChunk, 1, 8, pFile);
	fwrite(&pFmtChunk->DataSize, 1, 4, pFile);
	// FMT Chunk data
	fwrite(reinterpret_cast<FormatChunkData*>(pFmtChunk->Data), 1, pFmtChunk->DataSize, pFile);

	// FACT Chunk if exists (opt)
	if(pFactChunk != NULL)
	{
		fwriteBigEndian(&pFactChunk->ID, 4, pFile);
		fwrite(&pFactChunk->DataSize, 1, 4, pFile);
		// FACT Chunk data
		fwrite(reinterpret_cast<FactChunkData*>(pFactChunk->Data)->Buffer, 1, pFactChunk->DataSize, pFile);
	}

	// Data Chunk
	//fwrite(pDataChunk, 1, 8, pFile);
	fwriteBigEndian(&pDataChunk->ID, 4, pFile);
	fwrite(&pDataChunk->DataSize, 1, 4, pFile);
	// Data Data Chunk
	fwrite(reinterpret_cast<DataChunkData*>(pDataChunk->Data)->Buffer, sizeof(BYTE), pDataChunk->DataSize, pFile);

	// file done, close
	fclose(pFile);

Error:
	return r;
}

RESULT SoundWave::OpenWaveFile()
{
    RESULT r = R_OK; 

    m_pszFilePath = new char[MAX_PATH * 2];

    WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, m_pwszPath, -1, m_pszFilePath, MAX_PATH * 2, NULL, NULL);
    strcat(m_pszFilePath, "\\data\\");
    strcat(m_pszFilePath, m_pszFilename);       

    fileWave = fopen(m_pszFilePath, "rb");
    CNRM(fileWave, "Could not open file\n");

    r = ReadChunk(fileWave, m_pRiffChunk);

    if(m_pRiffChunk->ID == CHUNK_ID_RIFF)
    {
        // Create the list of chunks
        list<Chunk*> *temp = new list<Chunk*>();     

        // Found correct RIFF header chunk now parse through the different chunks available
        while(!feof(fileWave))
        {
            Chunk *pTempChunk;
            r = ReadChunk(fileWave, pTempChunk);

            if(pTempChunk != NULL)
            {
                temp->Append(pTempChunk);
            }
        }        

        reinterpret_cast<RiffChunkData*>(m_pRiffChunk->Data)->ChunkList = temp;
    }
    else
    {
        printf("ERROR: RIFF Header Chunk not found, is this a valid WAV file format?\n");
    }

Error:
    return r;
}

FormatChunkData* SoundWave::GetFormatChunk()
{
    list<Chunk*> *temp = reinterpret_cast<RiffChunkData*>(m_pRiffChunk->Data)->ChunkList;

    // Look for the Format Chunk
    list<Chunk*>::iterator it = temp->First();

    while(reinterpret_cast<Chunk*>(*it)->ID != CHUNK_ID_FMT )
    {
        it++;
    }

    return reinterpret_cast<FormatChunkData*>(reinterpret_cast<Chunk*>(*it)->Data);
}

// Format Interface Functions
COMPRESSION_CODE SoundWave::GetCompressionCode()
{
    FormatChunkData *fmt = GetFormatChunk();
    // For some reason it may be a good idea
    // to make sure we have a valid format chunk here
    // to avoid a catastrophic crash
    if(fmt != NULL)
    {
        switch(fmt->CompressionCode)
        {
            case WAVE_COMPRESSION_CODE_UNKNOWN:         return COMPRESSION_CODE_UNKNOWN; break;
            case WAVE_COMPRESSION_CODE_PCM:             return COMPRESSION_CODE_PCM; break;
            case WAVE_COMPRESSION_CODE_MSFT_ADPCM:      return COMPRESSION_CODE_MSFT_ADPCM; break;            
            case WAVE_COMPRESSION_CODE_MPEG:            return COMPRESSION_CODE_MPEG; break;
            case WAVE_COMPRESSiON_CODE_EXPERIMENTAL:    return COMPRESSiON_CODE_EXPERIMENTAL; break;
            default:                                    return COMPRESSION_CODE_UNKNOWN; break;
        }        
    }
    else
        return COMPRESSION_CODE_UNKNOWN;
}

int SoundWave::GetChannels()
{
    FormatChunkData *fmt = GetFormatChunk();
    if(fmt != NULL)
        return fmt->Channels;
    else
        return 2;
}

int SoundWave::GetSampleRate()
{
    FormatChunkData *fmt = GetFormatChunk();
    if(fmt != NULL)
        return fmt->SampleRate;
    else
        return 44100;
}

long SoundWave::GetAvgBytesPerSecond()
{
    FormatChunkData *fmt = GetFormatChunk();
    if(fmt != NULL)
        return fmt->AvgBytesPerSecond;
    else
        return 44100 * 4;
}

int SoundWave::GetBlockAlign()
{
    FormatChunkData *fmt = GetFormatChunk();
    if(fmt != NULL)
        return fmt->BlockAlign;
    else
        return 4;
}

int SoundWave::GetBitsPerSample()
{
    FormatChunkData *fmt = GetFormatChunk();
    if(fmt != NULL)
        return fmt->SigBitsPerSample;
    else
        return 16;
}

long SoundWave::GetSampleCount()
{
    long DataSizeSum = 0;
    
    list<Chunk*> *temp = reinterpret_cast<RiffChunkData*>(m_pRiffChunk->Data)->ChunkList;

    // Look for the Format Chunk
    list<Chunk*>::iterator it = temp->First();

    while(&it != NULL && *it != NULL)
    {
        if((*it)->ID == CHUNK_ID_DATA)        
            DataSizeSum += (*it)->DataSize;        

        it++;
    }

    return DataSizeSum;
}

SoundWave::~SoundWave()
{
    RESULT r = R_OK;
    // Need to delete the chunks etc!

    // Kill the Implementation
    if (m_pSoundImp != NULL)
    {
        m_pSoundImp->Release();
        delete m_pSoundImp;
    }

    if(m_pRiffChunk != NULL)
    {
        r = DestroyWaveFile();
    }

    // Close the file if it isn't already closed
    if(fileWave)
    {
        fclose(fileWave);
    }
}

SoundWave::SoundWave( const char* pszFilename , SoundImpFactory *pSIF) :
    fileWave(NULL),
    m_pSoundImp(NULL),
    m_pRiffChunk(NULL),
    m_SoundImpFactory(pSIF)
{
    RESULT r = R_OK;

    m_pwszPath = new wchar_t[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, m_pwszPath);

    m_pszFilename = new char[strlen(pszFilename)];
    strcpy(m_pszFilename, pszFilename);

    r = OpenWaveFile();

    // Set up the implementation
    if(m_SoundImpFactory)
    {
        m_pSoundImp = m_SoundImpFactory->MakeSoundImp();
        m_pSoundImp->Initialize(this);
    }
    else
    {
        printf("WARNING: SoundWave not complete since factory is NULL\n");
    }
}

SoundWave::SoundWave(SoundBuffer *pSoundBuffer, SoundImpFactory *pSIF) :
	fileWave(NULL),
	m_pSoundImp(NULL),
	m_pRiffChunk(NULL),
	m_SoundImpFactory(pSIF)
{
	RESULT r = R_OK;

	m_pwszPath = NULL;
	m_pszFilename = NULL;

	r = CreateFromBuffer(pSoundBuffer);

	// Set up the implementation
	if(m_SoundImpFactory)
	{
		m_pSoundImp = m_SoundImpFactory->MakeSoundImp();
		m_pSoundImp->Initialize(this);
	}
	else
	{
		printf("WARNING: SoundWave not complete since factory is NULL\n");
	}
}

void SoundWave::Print()
{
    printf("Sound Wave : Sound Obj \n");
}

RESULT SoundWave::StreamData(long Offset)
{
    RESULT r = R_NOT_IMPLEMENTED;

    /*TODO*/

Error:
    return r;
}

RESULT SoundWave::DestroyWaveFile()
{
    RESULT r = R_OK;

    list<Chunk*> *TempList = reinterpret_cast<RiffChunkData*>(m_pRiffChunk->Data)->ChunkList;

    while(TempList->Size() > 0)
    {
        Chunk* TempChunk = TempList->Pop();
        CNRM(TempChunk, "TempChunk is NULL");

        switch(TempChunk->ID) 
        {                    
            case CHUNK_ID_FMT:
            {
                // Check for any "extra" data
                if(reinterpret_cast<FormatChunkData*>(TempChunk->Data)->ExtraBytes > 0)
                {
                    delete reinterpret_cast<FormatChunkData*>(TempChunk->Data)->ExtraBuffer;
                }
                delete TempChunk->Data;
                delete TempChunk;
            } break;
            
            case CHUNK_ID_DATA:
            {            
                if(TempChunk->DataSize > 0)
                {                
                    delete reinterpret_cast<DataChunkData*>(TempChunk->Data)->Buffer;
                }
                delete TempChunk->Data;
                delete TempChunk;
            } break;

            case CHUNK_ID_FACT:
            {
                if(TempChunk->DataSize > 0)
                {
                    delete reinterpret_cast<FactChunkData*>(TempChunk->Data)->Buffer;                    
                }
                delete TempChunk->Data;
                delete TempChunk;
            } break;

            default:
            {
                printf("Found chunk in list that is unidentifiable!\n");
            } break;
        }
    }

    // Should be done at this point
    CBRM(TempList->Size() == 0, "List not empty!\n");

    delete TempList;
    delete m_pRiffChunk;

Error:
    return r;
}

// Play data will send the Wave File data to the DirectSound object
// and will lump together all the different chunks into one memory location.
RESULT SoundWave::PlayData(long Offset)
{
    RESULT r = R_OK;
    list<Chunk*> *TempList = reinterpret_cast<RiffChunkData*>(m_pRiffChunk->Data)->ChunkList;

    void* StaticBuffer = NULL;
    int StaticBuffer_n = 0;

    for(list<Chunk*>::iterator it = TempList->First(); it != NULL; it++)
    {
        if(reinterpret_cast<Chunk*>(*it)->ID == CHUNK_ID_DATA)
        {
            void* BufferPtr = reinterpret_cast<DataChunkData*>(reinterpret_cast<Chunk*>(*it)->Data)->Buffer;
            int BufferPtr_n = reinterpret_cast<Chunk*>(*it)->DataSize;
            r = ExpandStaticBuffer(StaticBuffer, StaticBuffer_n, BufferPtr, BufferPtr_n);
            CRM(r, "Could not expand buffer!");
        }
    }

    CNRM(StaticBuffer, "SoundWave::Play Buffer is NULL");

    m_pSoundImp->PassBufferToImp(StaticBuffer, StaticBuffer_n);

    m_pSoundImp->PlayBuffer(Offset, false);

Error:
    return r;
}

// Will return the sound status 
// allocates the memory for the Sound Status 
// structure
RESULT SoundWave::GetSoundStatus(SoundStatus* &n_pSS)
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

RESULT TestSoundWave(Console *pc, char *pszFilename)
{
    RESULT r = R_OK;

    SoundImpFactory sif(SOUND_IMP_DIRECTSOUND);
    //SoundWave pSoundWave("HipHoppy_1.wav", &sif);
	SoundWave pSoundWave(pszFilename, &sif);
    
	/*
    reinterpret_cast<SoundImpDirectSound*>(pSoundWave.m_pSoundImp)->CreatePositionNotification(2500);
    reinterpret_cast<SoundImpDirectSound*>(pSoundWave.m_pSoundImp)->CreatePositionNotification(25000);
    reinterpret_cast<SoundImpDirectSound*>(pSoundWave.m_pSoundImp)->CreatePositionNotification(250000);
    reinterpret_cast<SoundImpDirectSound*>(pSoundWave.m_pSoundImp)->CreatePositionNotification(350000);
    reinterpret_cast<SoundImpDirectSound*>(pSoundWave.m_pSoundImp)->CreatePositionNotification(450000);
    reinterpret_cast<SoundImpDirectSound*>(pSoundWave.m_pSoundImp)->CreatePositionNotification(550000); 
	*/

    //reinterpret_cast<SoundImpDirectSound*>(pSoundWave.m_pSoundImp)->RegisterPositionNotifications();

    pSoundWave.PlayData(0);

    while(pSoundWave.SoundPlaying())
    {
        SoundStatus* pSS = NULL;
        pSoundWave.GetSoundStatus(pSS);

        printf("Current Position: %d\r", pSS->PlayOffset);

        reinterpret_cast<SoundImpDirectSound*>(pSoundWave.m_pSoundImp)->CheckPositionNotifications();
    }

    system("PAUSE");

Error:
    return r;
}