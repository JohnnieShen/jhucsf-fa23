#include <iostream>
#include <string.h>
#include <vector>
#include <sstream>
#include <cmath>
#include <bit>
#include <bitset>
#include <cstdint>
#include <climits>

using namespace std;

struct cache_slot{
    uint32_t tag;
    bool valid;
    bool dirty;
    uint32_t load_ts;
    uint32_t access_ts;
    cache_slot() : tag(0), valid(false), dirty(false), load_ts(0), access_ts(0) {}
};

struct cache_set{
    vector<cache_slot> slots;
    cache_set() : slots() {}
    cache_set(int num_slots) : slots(num_slots) {}
};

struct Cache{
private:
    uint32_t num_sets;
    uint32_t num_blocks;
    uint32_t block_size;
    uint32_t set_offset;
    uint32_t set_bits;
    uint32_t tag_offset;

    std::vector<cache_set> cache_sets;

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
    Cache(uint32_t num_sets, uint32_t num_blocks, uint32_t block_size, bool writeAllocate, bool writeThrough, bool useLRU) 
        : num_sets(num_sets), num_blocks(num_blocks), block_size(block_size), cache_sets(num_sets, cache_set(num_blocks)), writeAllocate(writeAllocate)
        , writeThrough(writeThrough), useLRU(useLRU), totalLoads(0), totalStores(0), loadHits(0), loadMisses(0), storeHits(0), storeMisses(0)
        , totalCycles(0){
            set_offset = int(log2(block_size));
            set_bits = int(log2(num_sets));
            tag_offset = set_offset + set_bits;
            //cache_sets.resize(num_sets);
            for(uint32_t i = 0;i<num_sets;i++) {
                cache_set& set = cache_sets[i];
                for (uint32_t j = 0;j<num_blocks;j++) {
                    set.slots[j].access_ts = j;
                }
            }
        }

    void access(pair<char, int> input) {
        bool hit = false;
        char type = input.first;
        int address = input.second;
        int index = getIndex(address);
        uint32_t tag = getTag(address);
        cache_set& set = cache_sets[index];
        int oldestSlot = -1;
        int emptySlot = -1;

        if(type == 'l') {
            totalLoads++;
        } else if (type == 's') {
            totalStores++;
        }

        for (uint32_t i = 0; i < num_blocks; i++) {
            if (set.slots[i].valid) {
                updateAccessTS(i, set);
                if (set.slots[i].tag == tag) {
                    hit = true;
                    if (type == 's') {
                        if (writeAllocate) {
                            loadWriteMemory();
                        }
                        if (writeThrough) {
                            loadWriteMemory();
                        } else {
                            set.slots[i].dirty = true;
                        }
                        storeHits++;
                    } else {
                        loadHits++;
                    }
                    totalCycles++;
                    break;
                }
            } else if (emptySlot == -1) {
                emptySlot = i;
            }
        }

        if(useLRU) {
            for (uint32_t i = 0; i < num_blocks; i++) {
                if (set.slots[i].access_ts == num_blocks-1) { // not so sure
                    oldestSlot = i;
                }
            }
        }
        
        if (!hit) {
        // Cache miss
            if (type == 'l') {
                loadMisses++;
            } else if(type == 's') {
                storeMisses++;
            }
            
            int targetSlot;

            if (emptySlot != -1) {
                targetSlot = emptySlot;
            } else {
                targetSlot = oldestSlot;
            }
            if (type == 'l') {
                loadWriteMemory();
                set.slots[targetSlot].valid = true;
                set.slots[targetSlot].tag = tag;
                set.slots[targetSlot].dirty = false;
                updateAccessTS(targetSlot, set);
                updateLoadTS(targetSlot, set);
                totalCycles++;
            } else if (type == 's') {
                if (writeAllocate) {
                    loadWriteMemory();
                    if (!writeThrough && set.slots[targetSlot].dirty) {
                        loadWriteMemory();
                    }
                    set.slots[targetSlot].valid = true;
                    set.slots[targetSlot].tag = tag;
                    updateAccessTS(targetSlot, set);
                    updateLoadTS(targetSlot, set);
                    set.slots[targetSlot].dirty = true; 
                    totalCycles++;
                    if (writeThrough) {
                        loadWriteMemory();
                    }
                } else {
                    if (writeThrough) {
                        loadWriteMemory();
                    }
                }
            }
        }
    }

    int getIndex(int address) {
        int shifted = address >> set_offset;
        return shifted & ((1 << set_bits) - 1);
    }

    uint32_t getTag(int address) {
        return address >> tag_offset;
    }

    void loadWriteMemory() {
        totalCycles += (block_size / 4) *100;
    }

    void updateAccessTS(int index, cache_set& set) {
        uint32_t targetTS = set.slots[index].access_ts;
        if(targetTS == 0) {
            return;
        }
        for(uint32_t i = 0;i<num_blocks;i++) {
            totalCycles++;
            if(targetTS > set.slots[i].access_ts) {
                set.slots[i].access_ts++;
            }
        }
        set.slots[index].access_ts = 0;
    }

    void updateLoadTS(int index, cache_set& set) {
        uint32_t targetTS = set.slots[index].load_ts;
        if(targetTS == 0) {
            return;
        }
        for(uint32_t i = 0;i<num_blocks;i++) {
            totalCycles++;
            if(targetTS > set.slots[i].load_ts) {
                set.slots[i].load_ts++;
            }
        }
        set.slots[index].load_ts = 0;
    }

    void printOutput() {
        cout<<"Total loads: "<<totalLoads<<endl;
        cout<<"Total stores: "<<totalStores<<endl;
        cout<<"Load hits: "<<loadHits<<endl;
        cout<<"Load misses: "<<loadMisses<<endl;
        cout<<"Store hits: "<<storeHits<<endl;
        cout<<"Store misses: "<<storeMisses<<endl;
        cout<<"Total cycles: "<<totalCycles<<endl;
    }
};

bool isPowerOfTwo(int n) {
    if(n <= 0) return false;
    return (n & (n - 1)) == 0;
}


pair<char, int> parseInput(const string& line) {
    istringstream iss(line);
    char operation;
    string address;

    iss >> operation >> address;

    return make_pair(operation, stoi(address, NULL, 16));
}


int main(int argc, char *argv[]) {
    int numSets;
    int numBlocks;
    int blockSize;
    bool writeAllocate;
    bool writeThrough;
    bool useLRU;
    if(argc != 7) {
        cout<<"incorrect arguments"<<endl;
        return 1;
    }
    try{
        numSets = stoi(argv[1],NULL,10);
        numBlocks = stoi(argv[2],NULL,10);
        blockSize = stoi(argv[3],NULL,10);
        writeAllocate = string(argv[4]) == "write-allocate";
        writeThrough = string(argv[5]) == "write-through";
        useLRU = string(argv[6]) == "lru";
    } catch(exception&) {
        cout<<"incorrect inputs";
        return 1;
    }
    if (isPowerOfTwo(numSets)== false) {
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
    
    // int testAddress = 0x12345678;
    // cout << "Index: " << cache.getIndex(testAddress) << endl;
    // cout << "Tag: " << cache.getTag(testAddress) << endl;
    string line;
    while (getline(cin, line)) {
        if (line.empty()) {
            break;
        }

        pair<char, int> input = parseInput(line);

        cache.access(input);
    }
    cache.printOutput();
}