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

struct myFile
{
    std::string name;
    int size;
};

struct commonData
{
    vector<unordered_set<int>> &precalculated;
    vector<myFile> &files;
    vector<vector<list<int>>> &mapperResults;

    // Synchronization primitives.
    pthread_barrier_t *barrierPhase;
    pthread_barrier_t *barrierPrecalc;
    pthread_mutex_t *canSelectFile;

    // Number of Reducers and Mappers.
    int &R;
    int &M;
    commonData(vector<unordered_set<int>> &v1,
               vector<myFile> &files,
               vector<vector<list<int>>> &map,
               int &r,
               int &m)
        : precalculated(v1),
          files(files),
          mapperResults(map),
          R(r),
          M(m) {}
};

struct threadData
{
    int id;
    commonData *data;
};