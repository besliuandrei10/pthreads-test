#include <iostream>
#include <stdio.h>
#include <fstream>
#include <pthread.h>
#include <cstdlib>
#include <cstring>
#include <unordered_set>
#include <vector>
#include <array>
#include <list>
#include <algorithm>
#include <climits>
#include <cmath>

using namespace std;

struct myFile {
    std::string name;
    int size;
};


struct commonData {
    vector<unordered_set<int>> &precalculated;
    vector<myFile> &files;
    vector<vector<list<int>>> &mapperResults;
    pthread_barrier_t *barrier;
    pthread_mutex_t *mutex;
    int &R;
    commonData(vector<unordered_set<int>> &v1,
              vector<myFile> &files,
              vector<vector<list<int>>> &map,
              int &r) 
              : 
                precalculated(v1),
                files(files),
                mapperResults(map),
                R(r) {}
};

struct threadData {
    int id;
    commonData *data;
};