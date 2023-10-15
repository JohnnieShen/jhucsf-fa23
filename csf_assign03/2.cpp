#include <iostream>
#include <vector>
#include <bitset>
#include <cmath>

// Struct for a single cache line
struct CacheLine {
    bool valid;
    bool dirty;
    int tag;
    int lastUsedTime;
    int addedTime;

    CacheLine() : valid(false), dirty(false), tag(0), lastUsedTime(0), addedTime(0) {}
};

// Class for the entire cache structure
class Cache {
private:
    std::vector<std::vector<CacheLine>> sets;
    int numSets;
    int numBlocks;
    int blockSize;
    bool writeAllocate;
    bool writeThrough;
    bool useLRU;

    // Statistics
    int totalLoads;
    int totalStores;
    int loadHits;
    int loadMisses;
    int storeHits;
    int storeMisses;
    int totalCycles;

    // Other important data like time, block mask, etc.
    int currentTime;
    int blockOffsetBitCount;
    int indexBitCount;
    int tagBitCount;

public:
    // Constructor to initialize the cache with given parameters
    Cache(int numSets, int numBlocks, int blockSize, bool writeAllocate, bool writeThrough, bool useLRU)
    : numSets(numSets), numBlocks(numBlocks), blockSize(blockSize),
      writeAllocate(writeAllocate), writeThrough(writeThrough), useLRU(useLRU),
      totalLoads(0), totalStores(0), loadHits(0), loadMisses(0),
      storeHits(0), storeMisses(0), totalCycles(0), currentTime(0) {
        sets.resize(numSets, std::vector<CacheLine>(numBlocks));

        blockOffsetBitCount = std::log2(blockSize);
        indexBitCount = std::log2(numSets);
        tagBitCount = 32 - indexBitCount - blockOffsetBitCount;
    }

    // Function to simulate access to the cache
    void accessCache(char operation, int address) {
        ++currentTime;
        bool isLoad = operation == 'l';

        // Extract tag, index from address
        int index = (address >> blockOffsetBitCount) & ((1 << indexBitCount) - 1);
        int tag = address >> (indexBitCount + blockOffsetBitCount);

        // Check all blocks in the relevant set
        for (CacheLine &line : sets[index]) {
            if (line.valid && line.tag == tag) { // cache hit
                if (isLoad) {
                    ++loadHits;
                    ++totalLoads;
                } else {
                    ++storeHits;
                    ++totalStores;
                    if (!writeThrough) {
                        line.dirty = true;  // mark block as dirty
                    }
                }
                line.lastUsedTime = currentTime;
                totalCycles += 1;  // cache hit takes 1 cycle
                return;
            }
        }

        // cache miss
        if (isLoad) {
            ++loadMisses;
            ++totalLoads;
        } else {
            ++storeMisses;
            ++totalStores;
        }
        totalCycles += 100;  // cache miss takes 100 cycles for memory access

        // On a write miss, if it's write-allocate, load the block to cache. If it's no-write-allocate, do nothing further.
        // On a load miss or if it's write-through, load the block to cache.
        if ((isLoad || (writeAllocate && !isLoad)) || (writeThrough && !isLoad)) {
            evictAndLoadNewBlock(index, tag);
        }
    }

    // Function to evict a block (if necessary) and load a new block into the cache
    void evictAndLoadNewBlock(int index, int tag) {
        CacheLine *lineToReplace = nullptr;

        // Find an invalid block or apply the LRU/FIFO policy
        for (CacheLine &line : sets[index]) {
            if (!line.valid) {
                lineToReplace = &line;
                break;
            }
        }

        if (!lineToReplace) {
            if (useLRU) {
                // LRU policy
                int oldestTime = currentTime;
                for (CacheLine &line : sets[index]) {
                    if (line.lastUsedTime < oldestTime) {
                        oldestTime = line.lastUsedTime;
                        lineToReplace = &line;
                    }
                }
            } else {
                // FIFO policy
                int earliestTime = currentTime;
                for (CacheLine &line : sets[index]) {
                    if (line.addedTime < earliestTime) {
                        earliestTime = line.addedTime;
                        lineToReplace = &line;
                    }
                }
            }
        }

        // If the block is dirty and it's write-back, write it to memory
        if (lineToReplace->dirty && !writeThrough) {
            totalCycles += 100;  // writing to memory takes 100 cycles
        }

        // Load the new block
        lineToReplace->valid = true;
        lineToReplace->dirty = false;
        lineToReplace->tag = tag;
        lineToReplace->lastUsedTime = currentTime;
        lineToReplace->addedTime = currentTime;
    }

    // Function to print the simulation results
    void printResults() {
        std::cout << "Total loads: " << totalLoads << std::endl;
        std::cout << "Total stores: " << totalStores << std::endl;
        std::cout << "Load hits: " << loadHits << std::endl;
        std::cout << "Load misses: " << loadMisses << std::endl;
        std::cout << "Store hits: " << storeHits << std::endl;
        std::cout << "Store misses: " << storeMisses << std::endl;
        std::cout << "Total cycles: " << totalCycles << std::endl;
    }
};

// Helper function to check if a number is a power of 2
bool isPowerOfTwo(int x) {
    return x && !(x & (x - 1));
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        std::cerr << "Error: Expected 6 arguments." << std::endl;
        return 1;
    }

    int numSets = std::stoi(argv[1]);
    int numBlocks = std::stoi(argv[2]);
    int blockSize = std::stoi(argv[3]);
    bool writeAllocate = std::string(argv[4]) == "write-allocate";
    bool writeThrough = std::string(argv[5]) == "write-through";
    bool useLRU = std::string(argv[6]) == "lru";

    // Validate the arguments
    if (!isPowerOfTwo(numSets) || !isPower