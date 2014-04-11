//
//  EnvelopeFollowerNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.

//  This class provides the means to follow the envelope of a signal.
//  The input signal should be centered at zero, with positive and negative
//  values. The envelope value will always be positive.


#ifndef gTarAudioController_EnvFollower_h
#define gTarAudioController_EnvFollower_h

#include <math.h>
#include <string.h>

class EnvelopeFollowerNode
{
public:
    EnvelopeFollowerNode();
    
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