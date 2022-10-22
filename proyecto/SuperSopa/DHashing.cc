#include "DHashing.hh"
#include <iostream>
#include <string>
#include <unistd.h>
using namespace std;

// Constructors
DHashing::DHashing() {}

DHashing::DHashing(int n) {
    size = n;
    hashTable = vector<string>(n,"-");
}

// Private functions
unsigned int DHashing::hash1(string key) {
    unsigned int hashVal = 0;

    for(int i=0; i<key.size(); i++) {
        hashVal = (37*hashVal + key[i]) % size;
    }

    return hashVal;
}

unsigned int DHashing::hash2(string key) {
    int hashVal = 0;

    for(int i=0; i<key.size(); i++) {
        hashVal = (29*hashVal + key[i]) % 7;
    }

    // mod prime number smaller than table size
    unsigned int index = 7 - (hashVal%7);
    return index;
}

// Public functions
void DHashing::insert(const string key)
{
    int posh1 = hash1(key);
    int posh2 = hash2(key);
    int index = 0;

    // find pos with formula finalPos = pos(hash1) + i*pos(hash2)
    for (int i=1; i<=size; ++i) {
        index = (posh1 + i*posh2) % size;
        if(hashTable[index]=="-" or hashTable[index]==key) {
            //posFound = true;
            hashTable[index] = key;
            return;
        }
    }
}

bool DHashing::searchWord(const string key)
{
    int posh1 = hash1(key);
    int posh2 = hash2(key);
    int index = 0;
    bool posFound = false;

    for (int i=1; i<=size and !posFound; ++i) {
        index = (posh1 + i*posh2) % size;
        posFound = (hashTable[index]==key);
    }
    
    return posFound;
}

int DHashing::getSize() {
    return size;
}