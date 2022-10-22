#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <set>
#include "wordSearch.hh"
#include "DHashing.hh"
#include <chrono>
using namespace std::chrono;
using namespace std;

//                  NO N NE E SE S SO O
vector<int> dirI = {-1,0,1,1,1,0,-1,-1};
vector<int> dirJ = {-1,-1,-1,0,1,1,1,0};

typedef pair<string,PosChars> Result;

// words must have 20 positions
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

//recursive call
void searchDictionaryWordsRec(vector<DHashing>& dHashing, WordSearch& wordSearch, int i, int j, list<Result>& result, BoolMatrix& visited, string& currentWord, PosChars& auxPos, int cont) {
    
    //cout << "entra" << endl;
    if(not visited[i][j]) {
        
        visited[i][j] = true;
        //creamos string auxiliar para ver si el nuevo prefijo existe
        string auxWord = currentWord;
        auxWord.push_back(wordSearch.toChar(i,j));
        int auxI, auxJ;
        if (cont == 1) {
            
            auxPos.push_back({i,j});
            for(int k = 0; k < 8; k++) {
                
                auxI = i + dirI[k];
                auxJ = j + dirJ[k];
                if(wordSearch.posOk(auxI,auxJ)) searchDictionaryWordsRec(dHashing, wordSearch, auxI, auxJ, result, visited, auxWord, auxPos, cont+1);
            }
            auxPos.pop_back();
        }
        else {
            
            bool exists = dHashing[10].searchWord(auxWord);
            // existe la palabra entera
            if (exists) {
                
                auxPos.push_back({i,j});
                result.push_back({auxWord,auxPos});
                //llamada recursiva
                for(int k = 0; k < 8; k++) {
                    
                    auxI = i + dirI[k];
                    auxJ = j + dirJ[k];
                    if(wordSearch.posOk(auxI,auxJ)) searchDictionaryWordsRec(dHashing, wordSearch, auxI, auxJ, result, visited, auxWord, auxPos, cont+1);
                }
                auxPos.pop_back();
            }
            else {
                
                int auxWordSize = auxWord.size();
                if (auxWordSize > 10) exists = dHashing[9].searchWord(auxWord);
                else exists = dHashing[auxWordSize-2].searchWord(auxWord);
                // es un prefijo
                if (exists) {
                    
                    auxPos.push_back({i,j});
                    for(int k = 0; k < 8; k++) {
                        
                        auxI = i + dirI[k];
                        auxJ = j + dirJ[k];
                        if(wordSearch.posOk(auxI,auxJ)) searchDictionaryWordsRec(dHashing, wordSearch, auxI, auxJ, result, visited, auxWord, auxPos, cont+1);
                    }
                    auxPos.pop_back();
                }
            }
        }
        visited[i][j] = false;
    }
}

// first call to recursive searching words
void searchingWords(WordSearch& wordSearch, vector<DHashing>& dictionary, int n, int i, int j, list<Result>& result) {
    
    BoolMatrix visited(n, BoolRow(n, false));
    string w = "";
    PosChars auxPos;
    searchDictionaryWordsRec(dictionary, wordSearch, i, j, result, visited, w, auxPos, 1);
}

int main() {
    srand(time(NULL));
    
    vector<string> dictionary;
    vector<set<string>> prefixes(10);

    string w;
    string pre;
    auto start = high_resolution_clock::now();
    while(cin>>w) {
        dictionary.push_back(w);
        int wSize = w.size();
        for (int i = 2; i < wSize; i++) {
            pre = w.substr(0,i);
            if (i > 10) prefixes[9].insert(pre);
            else prefixes[i-2].insert(pre);
        }
    }
    auto stop = high_resolution_clock::now();
    auto duration_prefix = duration_cast<microseconds>(stop-start);
    cout << "prefix: " << duration_prefix.count() << endl;

    vector<string> words(20);
    chooseWordsFromDictionary(dictionary,words);
    
    // initialize the word search
    int n = rand() % 16 + 10;
    n = 50;
    WordSearch wordSearch(n);   
    for (int i = 0; i < 20; i++) {
        wordSearch.addWord(words[i]);
    }

    int wordsSize = words.size();
    for (int i = 0; i < wordsSize; i++) cout << words[i] << "  ";
    cout << endl << endl;

    // word search created
    wordSearch.print();

    // initialization started
    start = high_resolution_clock::now();
    
    vector<DHashing> vDHashing(11);
    int dicSize = dictionary.size();
    //real words
    vDHashing[10] = DHashing(dicSize);
    int setSize;

    for(int i = 0; i < 10; i++) {
        
        setSize = prefixes[i].size();
        //prefixes
        vDHashing[i] = DHashing(setSize);
    }

    // insert dictionary words
    for (int i = 0; i < dicSize; i++) vDHashing[10].insert(dictionary[i]);



    // insert prefixes in its correspondent DHashing vector
    for (int i = 0; i < 10; i++) {
        setSize = prefixes[i].size();
        for (auto it : prefixes[i]) vDHashing[i].insert(it);
    }


    stop = high_resolution_clock::now();
    auto duration_initalization = duration_cast<microseconds>(stop-start);
    cout << "initialization: " << duration_initalization.count() << endl;
    // initialization ended

    // finding started
    start = high_resolution_clock::now();
    cout << "searching" << endl;
    list<Result> wordsFound;
    for(int i = 0; i < n; i++) {    
        for(int j = 0; j < n; j++) {     
            searchingWords(wordSearch, vDHashing, n, i, j, wordsFound);
        }
    }

    stop = high_resolution_clock::now();
    auto duration_finding = duration_cast<milliseconds>(stop-start);
    cout << "finding: " << duration_finding.count() << endl;
    // finding ended
    
    // print words found in the word search
    /*
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
    */
    
}
