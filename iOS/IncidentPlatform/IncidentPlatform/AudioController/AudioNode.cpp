//
//  AudioNode.cpp
//  AudioController
//
//  Created by Idan Beck on 2/13/14.
//
//

#include "AudioNode.h"
#include <math.h>

/******************************************************/
// AudioNodeConnection
/******************************************************/

AudioNodeConnection::AudioNodeConnection() :
    m_connections(NULL)
{
    m_connections = new list<AudioNodeConnection*>();
}

AudioNodeConnection::~AudioNodeConnection() {
    if(m_connections != NULL) {
        delete m_connections;
        m_connections = NULL;
    }
}

RESULT AudioNodeConnection::Disconnect(AudioNodeConnection* audioConn) {
    RESULT r = R_SUCCESS;
    
    // Remove from connection list
    if(m_connections != NULL && m_connections->length() > 0) {
        CRM(m_connections->Remove((void*)(audioConn), GET_BY_ITEM), "Failed to disconnect node %d from connections", audioConn->m_node->GetID());
    }
    
Error:
    return r;
}

RESULT AudioNodeConnection::Disconnect() {
    RESULT r = R_SUCCESS;
    
    // Remove from all connections
    /*for(list<AudioNodeConnection*>::iterator it = m_connections->First(); it != NULL; it++) {
        CR((*it)->Disconnect(this));
    }*/
    
    while(m_connections->length() > 0) {
        AudioNodeConnection *tempConn = m_connections->Pop();
        
        DEBUG_LINEOUT("Disconnecting node %d from node %d", this->m_node->GetID(), tempConn->m_node->GetID());
        
        tempConn->Disconnect(this);
    }
    
Error:
    return r;
}

RESULT AudioNodeConnection::Connect(AudioNodeConnection* audioConn) {
    RESULT r = R_SUCCESS;
    
    CRM(m_connections->Append(audioConn), "Failed to connect node %d to node %d", this->m_node->GetID(), audioConn->m_node->GetID());
    DEBUG_LINEOUT("Connecting node %d to node %d", this->m_node->GetID(), audioConn->m_node->GetID());
    
Error:
    return r;
}

inline float AudioNodeConnection::GetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;

    for(list<AudioNodeConnection*>::iterator it = m_connections->First(); it != NULL; it++) {
        retVal += ((*it)->m_node->GetNextSample(timestamp)) * (*it)->m_gain;
    }

    
    // apply gain and return
    return (retVal * m_gain);
}


/******************************************************/
// Audio Node
/******************************************************/
AudioNode::AudioNode() :
    m_channel_in_n(0),
    m_channel_out_n(0),
    m_SampleRate(DEFAULT_SAMPLE_RATE),
    m_inputs(NULL),
    m_outputs(NULL),
    m_id(0)
{
    m_id = AudioNode::GetUniqueId();
}

float AudioNode::GetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;
    
    for(int i = 0; i < m_channel_in_n; i++) {
        retVal += (m_inputs[i]->GetNextSample(timestamp));
    }
    
    return retVal;
}

int AudioNode::GetUniqueId() {
    static int uid = 0;
    return uid++;
}
           
int AudioNode::GetID() {
    return  m_id;
}

RESULT AudioNode::DeleteAndDisconnect(CONN_TYPE type) {
    RESULT r = R_SUCCESS;
    
    AudioNodeConnection** connList = ((type == CONN_IN) ? m_inputs : m_outputs);
    int channels = ((type == CONN_IN) ? m_channel_in_n : m_channel_out_n);
    
    CBRM((type < CONN_INV), "Only input and output allowed connection types");
    
    if(connList != NULL) {
        for(int i = 0; i < channels; i++) {
            connList[i]->Disconnect();
            //delete connList[i];
            connList[i] = NULL;
        }
        
        delete [] connList;
        connList = NULL;
    }
    
Error:
    return r;
}

AudioNode::~AudioNode() {
    if(m_outputs != NULL) {
        DeleteAndDisconnect(CONN_OUT);
        m_outputs = NULL;
    }
    
    if(m_inputs != NULL) {
        DeleteAndDisconnect(CONN_IN);
        m_inputs = NULL;
    }
}

// Setting the channel will disconnect all current connections
RESULT AudioNode::SetChannelCount(int channel_n, CONN_TYPE type) {
    RESULT r = R_SUCCESS;
    AudioNodeConnection** connList = ((type == CONN_IN) ? m_inputs : m_outputs);
    int channels = ((type == CONN_IN) ? m_channel_in_n : m_channel_out_n);
    
    CBRM((type < CONN_INV), "Only input and output allowed connection types");
    
    if(connList != NULL) {
        for(int i = 0; i < channels; i++) {
            connList[i]->Disconnect();
            delete connList[i];
            connList[i] = NULL;
        }
        
        delete [] connList;
        connList = NULL;
    }
    
    if(type == CONN_IN) {
        m_inputs = new AudioNodeConnection*[channel_n];
        for(int i = 0; i < channel_n; i++)
            m_inputs[i] = new AudioNodeConnection();
        
        connList = m_inputs;
    }
    else if(type == CONN_OUT) {
        m_outputs = new AudioNodeConnection*[channel_n];
        for(int i = 0; i < channel_n; i++)
            m_outputs[i] = new AudioNodeConnection();
        
        connList = m_outputs;
    }
    else
        return R_ERROR;
    
    //m_channel_n = channel_n;
    if(type == CONN_IN)
        m_channel_in_n = channel_n;
    else if(type == CONN_OUT)
        m_channel_out_n = channel_n;
    
    // Set everything to zero, provide channel id
    for(int i = 0; i < channel_n; i++) {
        //memset(&(connList[i]), 0, sizeof(AudioNodeConnection));
        connList[i]->m_channel = i;
        connList[i]->m_node = this;
        connList[i]->m_gain = 1.0f;
    }
       
Error:
    return r;
}

RESULT AudioNode::SetChannelGain(int chan, float gain, CONN_TYPE type) {
    RESULT r = R_SUCCESS;
    
    if(type == CONN_OUT) {
        CBR((chan < m_channel_out_n));
        m_outputs[chan]->m_gain = gain;
    }
    else if(type == CONN_IN) {
        CBR((chan < m_channel_in_n));
        m_inputs[chan]->m_gain = gain;
    }
    
Error:
    return r;
}

RESULT AudioNode::SetChannelGain(float gain, CONN_TYPE type) {
    if(type == CONN_OUT)
        for(int i = 0; i < m_channel_out_n; i++)
            m_outputs[i]->m_gain = gain;
    else if(type == CONN_IN)
        for(int i = 0; i < m_channel_in_n; i++)
            m_inputs[i]->m_gain = gain;
    
    return R_SUCCESS;
}

AudioNodeConnection* AudioNode::GetChannel(int chan, CONN_TYPE type) {
    if(type == CONN_OUT) {
        if(chan >= m_channel_out_n)
            return NULL;
        else
            return (m_outputs[chan]);
    }
    else if(type == CONN_IN) {
        if(chan >= m_channel_in_n)
            return NULL;
        else
            return (m_inputs[chan]);
    }
    else
        return NULL;
}

RESULT AudioNode::ConnectInput(int inputChannel, AudioNode *inputNode, int outputChannel) {
    RESULT r = R_SUCCESS;
    
    AudioNodeConnection *inConn = GetChannel(inputChannel, CONN_IN);
    AudioNodeConnection *outConn = inputNode->GetChannel(outputChannel, CONN_OUT);
    
    CBRM((inConn != NULL), "AudioNode: Input channel not found");
    CBRM((outConn != NULL), "AudioNode: Output channel not found");
    
    // Connect the two
    CRM(inConn->Connect(outConn), "Failed to connect output %d to input %d", outConn->m_node->GetID(), inConn->m_node->GetID());
    CRM(outConn->Connect(inConn), "Failed to connect input %d to output %d", inConn->m_node->GetID(), outConn->m_node->GetID());
    
Error:
    return r;
}
