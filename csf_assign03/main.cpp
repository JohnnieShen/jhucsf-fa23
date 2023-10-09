#include <iostream>
#include <string.h>

using namespace std;

struct block{
private:
    int index;
    bool valid;
    int tag;
    int memoryMapped;

public:
    block(int index){
        this->index = index;
    }

    int loadData(){
        if(valid){
            return 1;
        }
        memoryMapped = tag + index; //needs to be modified
    }

    bool isValid(){
        return valid;
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

    int access = 0;
    int load = 0;
    int store = 0;
    int hit = 0;
    int miss = 0;

}