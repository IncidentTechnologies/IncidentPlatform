#include "EnvelopeFollower.h"


EnvelopeFollower::EnvelopeFollower()
{
    envelope=0;
}

void EnvelopeFollower::Setup( double attackMs, double releaseMs, int sampleRate )
{
    attack = pow( 0.01, 1.0 / ( attackMs * sampleRate * 0.001 ) );
    release = pow( 0.01, 1.0 / ( releaseMs * sampleRate * 0.001 ) );
}

