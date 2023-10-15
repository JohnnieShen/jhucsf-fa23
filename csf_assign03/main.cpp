#include <iostream>
#include <string.h>
#include <vector>
#include <cmath>

using namespace std;

struct Slot{
    uint32_t tag;
    bool valid;
    bool dirty;
    uint32_t load_ts;
    uint32_t access_ts;
    Slot() : tag(0), valid(false), dirty(false), load_ts(0), access_ts(0) {}
};

struct Set{
    vector<Slot> slots;
    Set(int num_slots) : slots(num_slots) {}
};

struct Cache{
private:
    std::vector<Set> sets;
    int num_sets;
    int num_blocks;
    int block_size;
    int set_index_bits;
    int block_offset_bits;

    bool writeAllocate;
    bool writeThrough;
    bool useLRU;
    
    int totalLoads;
    int totalStores;
    int loadHits;
    int loadMisses;
    int storeHits;
    int storeMisses;
    int totalCycles;

public:
    Cache(int num_sets, int num_blocks, int block_size, bool writeAllocate, bool writeThrough, bool useLRU) 
        : num_sets(num_sets), num_blocks(num_blocks), block_size(block_size), sets(num_sets, Set(num_blocks)), writeAllocate(writeAllocate)
        , writeThrough(writeThrough), useLRU(useLRU), totalLoads(0), totalStores(0), loadHits(0), loadMisses(0), storeHits(0), storeMisses(0)
        , totalCycles(0){
        set_index_bits = int(log2(num_sets));
        block_offset_bits = int(log2(block_size));
        sets.resize(num_sets);
    }

    void access(char type, uint32_t address) {
        
    }
};

bool isPowerOfTwo(int n) {
    if(n <= 0) return false;
    return (n & (n - 1)) == 0;
}




int main(int argc, char *argv[]) {
    int numSets = stoi(argv[1],NULL,10);
    int numBlocks = stoi(argv[2],NULL,10);
    int blockSize = stoi(argv[3],NULL,10);
    bool writeAllocate = string(argv[4]) == "write-allocate";
    bool writeThrough = string(argv[5]) == "write-through";
    bool useLRU = string(argv[6]) == "lru";
    if(argc != 7) {
        cout<<"incorrect arguments"<<endl;
        return 1;
    } else if (isPowerOfTwo(numSets)== false) {
        cout<<"number of sets is not a power of 2"<<endl;
        return 1;
    } else if (blockSize < 4) {
        cout<<"block size is less than 4"<<endl;
        return 1;
    } else if (isPowerOfTwo(blockSize) == false) {
        cout<<"block size is not a power of 2"<<endl;
        return 1;
    } else if (strcmp(argv[4], "no-write-allocate") == 0) {
        if (strcmp(argv[5], "write-back") == 0) {
            cout<<"write-back and no-write-allocate were both specified"<<endl;
            return 1;
        }
    }

    //cache initialization
    Cache cache = Cache(numSets, numBlocks, blockSize, writeAllocate, writeThrough, useLRU);
    
}