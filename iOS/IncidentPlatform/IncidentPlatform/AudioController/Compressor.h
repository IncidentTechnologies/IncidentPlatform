//
//  Compressor.h
//  gTarAudioController
//
//  Created by Franco Cedano on 1/16/12.
//  Copyright (c) 2012 Incident Technologies. All rights reserved.
//  
//  Compresses the dynamic range of an audio signal. Makes use of
//  an envelope follower to determine when the signal is above a given
//  threshold

#ifndef gTarAudioController_Compressor_h
#define gTarAudioController_Compressor_h

#import "EnvelopeFollower.h"

struct Compressor
{
    Compressor(double threshold, double ratio, double attackMs, double releaseMs, int sampleRate );
    void Setup(double threshold, double ratio, double attackMs, double releaseMs, int sampleRate );
    
    //  The numChannels parameter indicates the number of channels for an interleaved
    //  audio signal. For single channel mono signal numChannels = 1
    template<class T, int numChannels>
    void Process( size_t count, T *dest )
    {
        double volatile en;
        while( count-- )
        {
            T v=*dest;
            // don't worry, this should get optimized
            e.Process<T, numChannels>( 1, &v );
            en = e.envelope;
            if( e.envelope > m_threshold )
            {
                // The (1 + (env -thrshld)) term is so that we incrementally
                // increase the compression ratio smoothly as the evn gets larger.
                // This creates a smooth "soft knee" compression around the threshold
                // rather than a sudden "hard knee" transition from the uncompressed
                // to the compressed region and vice-versa
                *dest=*dest/(1 + (e.envelope - m_threshold) * m_ratio);
            }
            
            dest+=numChannels;
        }
    }
    
private:
    EnvelopeFollower e;
    double m_threshold;
    double m_ratio;
};

#endif
