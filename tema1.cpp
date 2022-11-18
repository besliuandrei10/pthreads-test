#include "tema1.h"
#define POW_OFFSET 2

using namespace std;

bool compareByLength(const myFile &a, const myFile &b)
{
    return a.size < b.size;
}

int myPow(const int &base, const int &pow)
{
    int output = 1;
    for (int i = 0; i < pow; i++)
    {
        output *= base;
    }

    return (output < 0) ? -1 : output;
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

bool myComparator(int a, int b) { return a < b; }

/*
 *  Reads filenames from given filename and adds them to the vector of files to be mapped from.
 */
vector<myFile> readInputFiles(const string &filename)
{
    // Open file containing mapper file names.
    ifstream f;
    f.open(filename);

    // Read number of entries.
    int n;
    f >> n;

    vector<myFile> files;
    for (auto i = 0; i < n; i++)
    {
        string mapperFile;
        f >> mapperFile;

        // Get file size.
        int file_size;

        ifstream in_file;
        in_file.open(mapperFile);
        in_file.seekg(0, ios::end);
        file_size = in_file.tellg();
        in_file.close();

        // Push file size to vector
        files.push_back(myFile());
        files[i].size = file_size;
        files[i].name = mapperFile;
    }

    sort(files.begin(), files.end(), compareByLength);
    return files;
}

/*
 * Mapper func.
 */
void *mapperFunction(void *arg)
{
    threadData *info = (threadData *)arg;
    commonData data = *(info->data);
    myFile currentFile;

    // Execute precalculations paralelized
    int start = info->id * (double)data.R / data.M;
    int end = min((info->id + 1) * (double)data.R / data.M, data.R);

    for (int i = start; i < end; i++)
    {
        int number = -1;
        int base = 1;
        int power = i + POW_OFFSET;

        do
        {
            number = myPow(base, power);
            data.precalculated[i].insert(number);
            base++;
        } while (number != -1);
    }

    pthread_barrier_wait(data.barrierPrecalc);

    // GET FILE TO WORK ON
    do
    {
        pthread_mutex_lock(data.canSelectFile);
        if (data.files.size() != 0)
        {
            currentFile = data.files.back();
            data.files.pop_back();
        }
        else
        {
            pthread_mutex_unlock(data.canSelectFile);
            break;
        }
        pthread_mutex_unlock(data.canSelectFile);

        ifstream f;
        f.open(currentFile.name);
        int n;
        f >> n;
        for (int i = 0; i < n; i++)
        {
            int number;
            f >> number;

            for (int pwrCheck = 0; pwrCheck < data.R; pwrCheck++)
            {

                if (data.precalculated[pwrCheck].find(number) != data.precalculated[pwrCheck].end())
                {
                    data.mapperResults[info->id][pwrCheck].push_back(number);
                }
            }
        }
        f.close();
    } while (1);

    pthread_barrier_wait(data.barrierPhase);
    return NULL;
}

/*
 * Reducer func.
 */
void *reducerFunction(void *arg)
{
    threadData *info = (threadData *)arg;
    commonData data = *(info->data);
    pthread_barrier_wait(data.barrierPhase);

    ofstream output;
    output.open("out" + to_string(info->id + POW_OFFSET) + ".txt");
    unordered_set<int> uniqueNumbers;

    for (auto map : data.mapperResults)
    {
        for (auto elem : map[info->id])
        {
            uniqueNumbers.insert(elem);
        }
    }
    output << uniqueNumbers.size();
    output.close();
    return NULL;
}

int main(int argc, char *argv[])
{

    // VARIABLES
    pthread_barrier_t barrierPhase;
    pthread_barrier_t barrierPrecalc;
    pthread_mutex_t canSelectFile;
    int M = 0;      // Number of Mappers.
    int R = 0;      // Number of Reducers.
    char *filepath; // Path to file.

    // Check for correct arguments.
    if (argc < 3)
    {
        printf("./tema1 <numar_mapperi> <numar_reduceri> <fisier_intrare>");
        return 0;
    }

    // Extract parameters.
    M = atoi(argv[1]);
    R = atoi(argv[2]);
    filepath = argv[3];

    // INIT AREA
    // Remove trailing newline. Too many nightmares because of this.
    filepath[strcspn(filepath, "\n")] = 0;

    // Create sorted list of mapper files
    vector<myFile> files = readInputFiles(filepath);

    // Precalculated powers vector
    vector<unordered_set<int>> precalcPowers(R, unordered_set<int>());

    // Lists of mapper outputs
    vector<vector<list<int>>> mapperResults(M, vector<list<int>>(R, list<int>()));

    // Create Thread arguments
    pthread_barrier_init(&barrierPhase, NULL, R + M);
    pthread_barrier_init(&barrierPrecalc, NULL, M);
    pthread_mutex_init(&canSelectFile, NULL);
    commonData data(precalcPowers, files, mapperResults, R, M);
    data.barrierPhase = &barrierPhase;
    data.barrierPrecalc = &barrierPrecalc;
    data.canSelectFile = &canSelectFile;

    // THREAD AREA
    pthread_t mappers[M];
    pthread_t reducers[R];
    threadData *mappersData = (threadData *)calloc(M, sizeof(threadData));
    threadData *reducersData = (threadData *)calloc(R, sizeof(threadData));
    int r;

    for (int id = 0; id < M; id++)
    {
        mappersData[id].id = id;
        mappersData[id].data = &data;
        r = pthread_create(&mappers[id], NULL, mapperFunction, (void *)&(mappersData[id]));

        if (r)
        {
            printf("Error when creating mapper %d\n", id);
            exit(-1);
        }
    }

    for (int id = 0; id < R; id++)
    {
        reducersData[id].id = id;
        reducersData[id].data = &data;
        r = pthread_create(&reducers[id], NULL, reducerFunction, (void *)&(reducersData[id]));

        if (r)
        {
            printf("Error when creating reducer %d\n", id);
            exit(-1);
        }
    }

    // JOINING THREADS
    void *status;
    for (int id = 0; id < M; id++)
    {
        r = pthread_join(mappers[id], &status);

        if (r)
        {
            printf("Error when waiting for mapper %d\n", id);
            exit(-1);
        }
    }

    for (int id = 0; id < R; id++)
    {
        r = pthread_join(reducers[id], &status);

        if (r)
        {
            printf("Error when waiting for reducer %d\n", id);
            exit(-1);
        }
    }

    // FREE AREA
    pthread_barrier_destroy(&barrierPhase);
    pthread_mutex_destroy(&canSelectFile);
    free(mappersData);
    free(reducersData);
    return 0;
}