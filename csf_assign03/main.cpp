#include <iostream>
#include <string.h>
#include <vector>

using namespace std;

struct Slot{
private:
    int index;
    bool valid;
    int tag;
    int accessTimeStamp;
    int loadTimeStamp;
    int data;

public:
    Slot(int index, int tag){
        this->index = index;
        this->valid = false;
        this->tag = tag;
        this->accessTimeStamp = -1;
        this->loadTimeStamp = -1;
        this->data = -1;
    }

    int loadData(){
        return data;
    }

    bool isValid(){
        return valid;
    }

};

struct Set{
private:
    vector<Slot> slots;
    int index;
public:
    Set(int size, int index){
        this->index = index;
        for(int i = 0; i < size; i++){
            slots.push_back(Slot(index, i));
        }
    }

    int getData(int tag){
        return slots[tag].loadData();
    }
};

struct Cache{
private:
    vector<Set> sets;

    int getIndex(string hexAddress){
        long address = stoul(hexAddress, nullptr, 16);
        long index = (address >> 2) & ((1 << 5) - 1);
        return (int) index;
    }

    int getTag(string hexAddress){
        long address = stoul(hexAddress, nullptr, 16);
        long tag = address >> 7;
        return (int) tag;
    }

public:
    Cache(int size, int setSize){
        for(int i = 0; i < size; i++){
            sets.push_back(Set(setSize, i));
        }
    }

    int getData(string hexAddress){
        int index = getIndex(hexAddress);
        int tag = getTag(hexAddress);
        Set currIndex = sets[index];
        return currIndex.getData(tag);
    }


};

bool isPowerOfTwo(int n) {
    if(n <= 0) return false;
    return (n & (n - 1)) == 0;
}

int main(int argc, char **argv) {
    int numSets = stoi(argv[1],NULL,10);
    int numBlocks = stoi(argv[2],NULL,10);
    int blockSize = stoi(argv[3],NULL,10);
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
    Cache cache = Cache(numSets, numBlocks);



    int access = 0;
    int load = 0;
    int store = 0;
    int hit = 0;
    int miss = 0;






}