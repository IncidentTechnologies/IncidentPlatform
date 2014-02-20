//
//  AudioNode.h
//  AudioController
//
//  Created by Idan Beck on 2/13/14.
//
//

#ifndef __AudioController__AudioNode__
#define __AudioController__AudioNode__

#include <iostream>
#include "dss_list.h"

#define DEFAULT_SAMPLE_RATE 44100.0f

using namespace dss;

class AudioNode;

typedef enum {
    CONN_IN,
    CONN_OUT,
    CONN_INV
} CONN_TYPE;

class AudioNodeConnection {
public:
    AudioNodeConnection();
    ~AudioNodeConnection();
    
    RESULT Disconnect();
    RESULT Disconnect(AudioNodeConnection* audioConn);
    
    RESULT Connect(AudioNodeConnection* audioConn);
    float GetNextSample(unsigned long int timestamp);
    
public:
    int m_channel;                     // channel
    float m_gain;                           // gain
    AudioNode *m_node;               // parent node
    list<AudioNodeConnection*> *m_connections;      // conn
};

class AudioNode {
public:
    AudioNode();
    ~AudioNode();
    
    RESULT SetChannelCount(int channel_n, CONN_TYPE type);
    AudioNodeConnection *GetChannel(int chan, CONN_TYPE type);
    virtual float GetNextSample(unsigned long int timestamp);
    RESULT ConnectInput(int inputChannel, AudioNode *inputNode, int outputChannel);
    
    RESULT DeleteAndDisconnect(CONN_TYPE type);
   
public:
    int m_SampleRate;
    
private:
    int m_channel_in_n;
    int m_channel_out_n;
    
    AudioNodeConnection** m_inputs;
    AudioNodeConnection** m_outputs;
    
    // Make it possible to search for nodes by id or name
    char *m_pszName;
    int m_id;
};

#endif /* defined(__AudioController__AudioNode__) */
