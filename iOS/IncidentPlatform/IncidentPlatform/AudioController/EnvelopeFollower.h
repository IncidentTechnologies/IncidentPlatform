//
//  EnvelopeFollower.h
//  gTarAudioController
//
//  Created by Franco Cedano on 1/16/12.
//  Copyright (c) 2012 Incident Technologies. All rights reserved.
//
//  This class provides the means to follow the envelope of a signal.
//  The input signal should be centered at zero, with positive and negative
//  values. The envelope value will always be positive.


#ifndef gTarAudioController_EnvFollower_h
#define gTarAudioController_EnvFollower_h

class EnvelopeFollower
{
public:
    EnvelopeFollower();
    
    void Setup( double attackMs, double releaseMs, int sampleRate );
    
    //  The numChannels parameter indicates the number of channels for an interleaved
    //  audio signal. For single channel mono signal numChannels = 1
    template<class T, int numChannels>
    void Process( size_t count, const T *src )
    {
        while( count-- )
        {
            double v=::fabs( *src );
            src+=numChannels;
            if( v>envelope )
                envelope = attack * ( envelope - v ) + v;
            else
                envelope = release * ( envelope - v ) + v;
        }
    }
    
    double envelope;
    
protected:
    double attack;
    double release;
};


#endif
