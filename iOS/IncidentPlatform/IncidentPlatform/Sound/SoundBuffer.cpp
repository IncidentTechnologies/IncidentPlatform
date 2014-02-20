#include "SoundBuffer.h"

SoundBuffer::SoundBuffer( float TimeLength, int SampleRate, SoundImpFactory *pSIF ) :
    m_floatBuffer(NULL),
    m_TimeLength(TimeLength),
    m_SampleRate(SampleRate),
    m_SoundImpFactory(pSIF)
{
    m_floatBuffer_n = TimeLength * SampleRate;
    m_floatBuffer = new float[m_floatBuffer_n];

    // Set up the implementation
    if(m_SoundImpFactory)
    {
        m_pSoundImp = m_SoundImpFactory->MakeSoundImp();
        m_pSoundImp->Initialize(this);
    }
    else
    {
        printf("WARNING: SoundBuffer not complete since factory is NULL\n");
    }
}

SoundBuffer::~SoundBuffer()
{
    RESULT r = R_OK;

    if(m_floatBuffer != NULL)
    {
        delete [] m_floatBuffer;
        m_floatBuffer = NULL;
    }

    // Kill the Implementation
    if (m_pSoundImp != NULL)
    {
        m_pSoundImp->Release();
        delete m_pSoundImp;
    }
}

RESULT SoundBuffer::PlayData( long Offset )
{
    RESULT r = R_OK;

    float *StaticBuffer = new float[m_floatBuffer_n];
    memcpy(StaticBuffer, m_floatBuffer, m_floatBuffer_n * sizeof(float));

    m_pSoundImp->PassBufferToImp(static_cast<void*>(StaticBuffer), m_floatBuffer_n * sizeof(float));
    m_pSoundImp->PlayBuffer(Offset, false);

Error:
    return r;
}

RESULT SoundBuffer::PassBuffer( float* &d_pBuffer, long &d_pBuffer_n )
{
    RESULT r = R_OK;

    CNRM(d_pBuffer, "SoundBuffer: Input buffer is NULL!");

    // first delete ours
    if(m_floatBuffer != NULL)
    {
        delete [] m_floatBuffer;
        m_floatBuffer = NULL;
    }

    m_floatBuffer_n = d_pBuffer_n;
    m_floatBuffer = new float[m_floatBuffer_n];
    memcpy(m_floatBuffer, d_pBuffer, d_pBuffer_n * sizeof(float));

    // Recalculate our time
    m_TimeLength = m_floatBuffer_n / m_SampleRate;

Error:
    // Delete input buffer
    delete [] d_pBuffer;
    d_pBuffer = NULL;
    d_pBuffer_n = 0;

    return r;
}

RESULT SoundBuffer::GetSoundStatus( SoundStatus* &n_pSS )
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

// This will output a comma delimited file of each value contained in the buffer
RESULT SoundBuffer::SaveBufferToFile(char *pszFilename)
{
    RESULT r = R_OK;

    FILE *pFile;
    pFile = fopen(pszFilename, "w");
    CNRM(pFile, "SoundBuffer: Failed to create %s", pszFilename);

    for(long i = 0; i < m_floatBuffer_n; i++)
    {
        fprintf(pFile, "%f\n", m_floatBuffer[i]);
    }
    //fprintf(pFile, "%f", m_floatBuffer[m_floatBuffer_n - 1]);

    fclose(pFile);

Error:
    return r;
}

RESULT SaveComplexFloatBufferToFile(ComplexFloatBuffer buf, char *pszFilename)
{
    RESULT r = R_OK;

    FILE *pFile;
    pFile = fopen(pszFilename, "w");
    CNRM(pFile, "SaveFloatBufferToFile: Failed to create %s", pszFilename);

    fprintf(pFile, "real, imag, abs, arg\n");

    // Real and Imaginary parts separated by comma
    for(long i = 0; i < buf.buffer_n; i++)
    {
        fprintf(pFile, "%f, ", real(buf.buffer[i]));
        fprintf(pFile, "%f,", imag(buf.buffer[i]));
        fprintf(pFile, "%f,", abs(buf.buffer[i]));
        fprintf(pFile, "%f\n", arg(buf.buffer[i]));
    }

    fclose(pFile);

Error:
    return r;
}

RESULT SoundBuffer::rFFT()
{
    RESULT r = R_OK;

    // First find out what exponent of 2 to use
    int pow2 = std::log((double)m_floatBuffer_n)/std::log(2.0f);
    int NewN = std::pow(2.0f, pow2);

    // First lets create a new FloatBuffer
    ComplexFloatBuffer buf;
    memset(&buf, 0, sizeof(ComplexFloatBuffer));
    buf.buffer = new std::complex<float>[NewN];

    for(long i = 0; i < NewN; i++)
    {
        buf.buffer[i] = std::complex<float>(m_floatBuffer[i], 0.0f);
    }

    buf.buffer_n = NewN;

    std::complex<float>temp(0.0, ((-2 * PI) / (float)(NewN)));
    //std::complex<float>temp(0.0, ((-2 * PI) / (float)(m_SampleRate)));

    std::complex<float>g = exp(temp);

    ComplexFloatBuffer res = rFFT(buf, g);

    SaveComplexFloatBufferToFile(res, "complexfloatbufferresult.csv");

Error:
    return r;
}

ComplexFloatBuffer SoundBuffer::rFFT(ComplexFloatBuffer fb, std::complex<float>g)
{
    long n = fb.buffer_n;
    
    if(n == 1)
        return fb;

    long m = n/2;

    // Set up the even buffer
    ComplexFloatBuffer Evens;
    memset(&Evens, 0, sizeof(ComplexFloatBuffer));
    Evens.buffer = new std::complex<float>[m];
    for(long i = 0; i < m; i++)
    {
        Evens.buffer[i] = fb.buffer[2*i];
    }
    Evens.buffer_n = m;

    // Set up the odd buffer
    ComplexFloatBuffer Odds;
    memset(&Odds, 0, sizeof(ComplexFloatBuffer));
    Odds.buffer = new std::complex<float>[m];
    for(long i = 0; i < m; i++)
    {
        Odds.buffer[i] = fb.buffer[2*i + 1];
    }
    Odds.buffer_n = m;

    // Apply Recursive FFT to Evens and Odds
    Evens = rFFT(Evens, g);
    Odds = rFFT(Odds, g);

    ComplexFloatBuffer Left;
    memset(&Left, 0, sizeof(ComplexFloatBuffer));
    Left.buffer = new std::complex<float>[n];
    for(long i = 0; i < n; i++)        
        Left.buffer[i] = Evens.buffer[i % m];
    Left.buffer_n = n;

    /*std::complex<float> g = */

    ComplexFloatBuffer Right;
    memset(&Right, 0, sizeof(ComplexFloatBuffer));
    Right.buffer = new std::complex<float>[n];
    for(long i = 0; i < n; i++)
    {
        std::complex<float> mul = pow(g, i);
        Right.buffer[i] = mul * Odds.buffer[i % m];    
    }
    Right.buffer_n = n;

    ComplexFloatBuffer ret;
    memset(&ret, 0, sizeof(ComplexFloatBuffer));
    ret.buffer = new std::complex<float>[n];
    for(long i = 0; i < n; i++)        
        ret.buffer[i] = Left.buffer[i] + Right.buffer[i];
    ret.buffer_n = n;

    return ret;        

}

RESULT SoundBuffer::GetSoundWave(SoundWave* &n_soundWave)
{
	RESULT r = R_OK;

	n_soundWave = new SoundWave(this, m_SoundImpFactory);

Error:
	return r;
}

/*
RESULT SaveToMp3File(char *pszFilepath, int bitrate)
{
	RESULT r = R_OK;

	

Error:
	return r;
}
*/