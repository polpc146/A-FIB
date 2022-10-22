#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include "wordSearch.hh"
#include "Trie.hh"
#include <chrono>
using namespace std::chrono;

//                  NO N NE E SE S SO O
vector<int> dirI = {-1,0,1,1,1,0,-1,-1};
vector<int> dirJ = {-1,-1,-1,0,1,1,1,0};

typedef pair<string,PosChars> Result;


void chooseWordsFromDictionary(vector<string> dictionary, vector<string>& words) {
    int dicSize = dictionary.size();
    vector<int> positionsToChoose(dicSize);
    for(int i=0; i<dicSize; ++i) positionsToChoose[i] = i;
    
    for(int i=0; i<20; ++i) {
        int r = rand() % (dicSize-i);
        words[i] = dictionary[r];

        swap(dictionary[r], dictionary[dicSize - (i+1)]);
    }
}

void searchDictionaryWordsRec(WordSearch& wordSearch, BoolMatrix& visited, Trie*& dictionary, list<Result>& result, PosChars& auxPos, string partialWord, string finalWord, int i, int j) {   
    if(not visited[i][j]) {
        visited[i][j] = true;
        partialWord.push_back(wordSearch.toChar(i,j));

        // check if complete word exists in the dictionary
        if (dictionary->search(partialWord)) {
            auxPos.push_back({i,j});
            finalWord.push_back(wordSearch.toChar(i,j));
            result.push_back({finalWord,auxPos});
            auxPos.pop_back();
        }
        else if(dictionary->existChildrenWithKey(partialWord)) {
            auxPos.push_back({i,j});
            Trie* childrenDictionary = dictionary->nodeWithKey(partialWord);
            int nexti, nextj;

            // recursive call
            for(int k = 0; k < 8; k++) {         
                nexti = i + dirI[k];
                nextj = j + dirJ[k];
                string resetWord = "";
                finalWord.push_back(wordSearch.toChar(i,j));
                if(wordSearch.posOk(nexti,nextj)) {
                    searchDictionaryWordsRec(wordSearch, visited, childrenDictionary, result, auxPos, resetWord, finalWord, nexti, nextj);
                }
                finalWord.pop_back();

            }
            auxPos.pop_back();
        }

        else if(dictionary->existsChildWithKeyPrefix(partialWord)) {
            auxPos.push_back({i,j});
            int nexti, nextj;

            // recursive call
            for(int k = 0; k < 8; k++) {         
                nexti = i + dirI[k];
                nextj = j + dirJ[k];
                finalWord.push_back(wordSearch.toChar(i,j));
                if(wordSearch.posOk(nexti,nextj)) {
                    searchDictionaryWordsRec(wordSearch, visited, dictionary, result, auxPos, partialWord, finalWord, nexti, nextj);
                }
                finalWord.pop_back();

            }
            auxPos.pop_back();
        }
        partialWord.pop_back();
        visited[i][j] = false;
    }
}

// first call to recursive searching words
void searchDictionaryWords(Trie*& dictionary, WordSearch& wordSearch, int i, int j, list<Result>& result) {
    BoolMatrix visited(wordSearch.getSize(), BoolRow(wordSearch.getSize(), false));
    string finalWord, partialWord = "";
    PosChars auxPos;
    searchDictionaryWordsRec(wordSearch, visited, dictionary, result, auxPos, partialWord, finalWord, i, j);
}

int main() {
    srand(time(NULL));
    
    vector<string> dictionary;
    string w;
    while(cin>>w) {
        dictionary.push_back(w);
    }
    
    vector<string> words(20);
    chooseWordsFromDictionary(dictionary,words);
    
    // initialize the word search
    int n = rand() % 16 + 10;
    WordSearch wordSearch(n);   
    for (int i = 0; i < 20; i++) {
        wordSearch.addWord(words[i]);
    }

    int wordsSize = words.size();
    for (int i = 0; i < wordsSize; i++) cout << words[i] << "  ";
    cout << endl << endl;

    // word search created
    wordSearch.print();

    // initialzation started

    // add to trie with words from dictionary
    Trie* trie = new Trie();
    for (int i = 0; i < dictionary.size(); ++i) {
        trie->insert(dictionary[i]);
    }
    // apply Patricia to trie
    trie->patricia();

    // initialization ended

    // finding started

    list<Result> wordsFound;
    for(int i = 0; i < n; i++) {      
        for(int j = 0; j < n; j++) {           
            searchDictionaryWords(trie, wordSearch, i, j, wordsFound);
        }
    }

    //finding ended

    // print words found in the word search
    cout << endl << "WORDS FOUND: " << wordsFound.size() << endl;
    
    list<Result>::const_iterator it;
    for(it = wordsFound.begin(); it!= wordsFound.end(); ++it) {       
        Result r = *it;
        cout << r.first << " -> ";
        int vsize = r.second.size();
        for (int i = 0; i < vsize; i++) {            
            cout << '{' << r.second[i].first << ',' << r.second[i].second << "} (" << wordSearch.toChar(r.second[i].first,r.second[i].second) << ")";
            if(i != vsize -1) cout << " , ";
        }
        cout << endl;   
    }
    cout << endl << "Words found: " << wordsFound.size() << endl;
}
