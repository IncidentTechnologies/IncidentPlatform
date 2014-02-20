#include "hash.h"
#include "string.h"

using namespace dss;

hash::hash() :
    hash_size(DEFAULT_HASH_SIZE),
    m_fnHash(default_hash_function)
{
    hash_data = new void*[hash_size];   
    memset(hash_data, 0, sizeof(void*) * hash_size);
}

hash::hash(fnHash hf) :
    hash_size(DEFAULT_HASH_SIZE)
{
    m_fnHash = hf;
    hash_data = new void*[hash_size];
    memset(hash_data, 0, sizeof(void*) * hash_size);
}

hash::~hash()  {
    for(int i = 0; i < hash_size; i++)
        if(hash_data[i] != 0)
            delete (void*)(hash_data[i]);
    
    delete hash_data;
}

int hash::add(void* data) {
    long int n = this->m_fnHash(data);
    
    if(hash_data[n] == 0) {
        hash_data[n] = data;
        return 0;
    }   
    else
        return 1;           // collision!

    return -1;
}

int hash::remove(void* data)
{
    long int n = this->m_fnHash(data);

    if(hash_data[n] != 0) {
        delete hash_data[n];
        return 0;
    }
    else
        return -1;
    
    return -1;
}

int hash::search(void* data) {
    // TODO: implement this!
    return -1;
}

long int hash::default_hash_function(void *key) {
    // Very simple hash function
    return ((long int)key % DEFAULT_HASH_SIZE);
}