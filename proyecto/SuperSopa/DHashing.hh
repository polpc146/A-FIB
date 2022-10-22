#include <iostream>
#include <string>
#include <vector>

using namespace std;

class DHashing {
    private:
        unsigned int hash1(string);
        unsigned int hash2(string);
        int size;
    
    public:
        vector<string> hashTable;
    
        // Constructor
        DHashing();
        DHashing(int n);

        void insert(const string);
        bool searchWord(const string);
        int getSize();
};
