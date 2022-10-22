#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <set>
#include "wordSearch.hh"
#include "BloomFilter.hh"

using namespace std;

//                  NO N NE E SE S SO O
vector<int> dirI = {-1,0,1,1,1,0,-1,-1};
vector<int> dirJ = {-1,-1,-1,0,1,1,1,0};

typedef pair<string,PosChars> Result;

bool dichotomousSearch(const string& auxWord, const vector<string>& dictionary, int left, int right) {
    
    if (left > right) return false;
    int m = (left + right) / 2;
    if (auxWord < dictionary[m]) return dichotomousSearch(auxWord, dictionary, left, m - 1);
    else if (auxWord > dictionary[m]) return dichotomousSearch(auxWord, dictionary, m + 1, right);
    //word found
    return true;
}

// words must have 20 positions
void chooseWordsFromDict(vector<string> dictionary, vector<string>& words) {
    
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
void searchingWordsRec(vector<BloomFilter>& vBloomFilter, WordSearch& wordSearch, int i, int j, list<Result>& result, BoolMatrix& visited, string& currentWord, PosChars& auxPos, int cont) {
    
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
                if(wordSearch.posOk(auxI,auxJ)) searchingWordsRec(vBloomFilter, wordSearch, auxI, auxJ, result, visited, auxWord, auxPos, cont+1);
            }
            auxPos.pop_back();
        }
        else {
            
            int exists = vBloomFilter[10].search(auxWord);
            // existe la palabra entera
            if (exists == 1) {
                
                auxPos.push_back({i,j});
                result.push_back({auxWord,auxPos});
                //llamada recursiva
                for(int k = 0; k < 8; k++) {
                    
                    auxI = i + dirI[k];
                    auxJ = j + dirJ[k];
                    if(wordSearch.posOk(auxI,auxJ)) searchingWordsRec(vBloomFilter, wordSearch, auxI, auxJ, result, visited, auxWord, auxPos, cont+1);
                }
                auxPos.pop_back();
            }
            else {
                
                int auxWordSize = auxWord.size();
                if (auxWordSize > 10) exists = vBloomFilter[9].search(auxWord);
                else exists = vBloomFilter[auxWordSize-2].search(auxWord);
                //es un prefijo
                if (exists == 1) {
                    
                    auxPos.push_back({i,j});
                    for(int k = 0; k < 8; k++) {
                        
                        auxI = i + dirI[k];
                        auxJ = j + dirJ[k];
                        if(wordSearch.posOk(auxI,auxJ)) searchingWordsRec(vBloomFilter, wordSearch, auxI, auxJ, result, visited, auxWord, auxPos, cont+1);
                    }
                    auxPos.pop_back();
                }
            }
        }
        visited[i][j] = false;
    }
}

//first call to recursive searching words
void searchingWords(vector<BloomFilter>& vBloomFilter, WordSearch& wordSearch, int n, int i, int j, list<Result>& result) {
    
    BoolMatrix visited(n, BoolRow(n, false));
    string w = "";
    PosChars auxPos;
    searchingWordsRec(vBloomFilter, wordSearch, i, j, result, visited, w, auxPos, 1);
}

int main() {
    
    srand(time(NULL));
    
    vector<string> dictionary;
    vector<set<string>> prefixes(10);
    string w;
    string pre;
    while(cin>>w) {
        
        dictionary.push_back(w);
        int wSize = w.size();
        for (int i = 2; i < wSize; i++) {
            
            pre = w.substr(0,i);
            if (i > 10) prefixes[9].insert(pre);
            else prefixes[i-2].insert(pre);
        }
    }
    vector<string> words(20);
    
    chooseWordsFromDict(dictionary,words);
    
    int n = rand() % 41 + 10;
    WordSearch wordSearch(n);
    
    // always 20 words to search
    for (int i = 0; i < 20; i++) {
        
        wordSearch.addWord(words[i]);
    }

    sort(words.begin(), words.end());
    int wordsSize = words.size();
    for (int i = 0; i < wordsSize; i++) cout << words[i] << "  ";
    cout << endl << endl;
    //aqui la sopa ya estaria creada y solo tocaria buscar las palabras
    wordSearch.print();
    
    vector<BloomFilter> vBloomFilter(11);
    int dicSize = dictionary.size();
    //real words
    vBloomFilter[10] = BloomFilter(dicSize, float(1/(dicSize*1.0)));
    int setSize;
    for(int i = 0; i < 10; i++) {
        
        setSize = prefixes[i].size();
        //prefixes
        vBloomFilter[i] = BloomFilter(setSize, float(1/(setSize*1.0)));
    }
    
    for (int i = 0; i < dicSize; i++) vBloomFilter[10].insertWord(dictionary[i]);
    for (int i = 0; i < 10; i++) {
        
        setSize = prefixes[i].size();
        for (auto it : prefixes[i]) vBloomFilter[i].insertWord(it);
    }
    
    list<Result> result;
    for(int i = 0; i < n; i++) {
        
        for(int j = 0; j < n; j++) {
            
            searchingWords(vBloomFilter, wordSearch, n, i, j, result);
        }
    }
    
    cout << endl << "WORDS FOUND" << endl << "------------------------------------------------------------" << endl;
    double resultSize = result.size();
    int cont = 0;
    list<Result>::const_iterator it;
    for(it = result.begin(); it!= result.end(); ++it) {
        
        Result r = *it;
        cout << r.first << " -> ";
        int vsize = r.second.size();
        for (int i = 0; i < vsize; i++) {
            
            cout << '{' << r.second[i].first << ',' << r.second[i].second << "}";
            if(i != vsize -1) cout << " , ";
        }
        cout << endl;
        if(dichotomousSearch(r.first, dictionary, 0, dictionary.size()-1)) ++cont;
    }
    cout << endl;
    double sol = double(cont/resultSize);
    cout << "correctesa: " << sol << endl;
}
