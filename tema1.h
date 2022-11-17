#include <iostream>
#include <fstream>
#include <pthread.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>

struct myFile {
    std::string name;
    int size;
};