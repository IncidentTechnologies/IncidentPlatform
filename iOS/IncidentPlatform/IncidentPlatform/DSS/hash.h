// Hash table implementation
// Default starting size is 25

#define DEFAULT_HASH_SIZE 25

namespace dss {

typedef long int (*fnHash)(void*);
    
class hash {
public:
    hash();
    hash(fnHash hash_function);
    ~hash();

    int add(void* data);
    int remove(void* data);
    int search(void* data);

    static long int default_hash_function(void* data);

private:
    int hash_size;
    void** hash_data;
    fnHash m_fnHash;
};

} // namespace dss