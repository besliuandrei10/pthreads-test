# pthreads-test
## **Task**

Calculate how many different numbers from given input files are perfect powers. The code checks for perfect powers up to the number of reducers specified when running the program.

## **Usage**

Compile using the included `makefile` and then execute using:

```
./tema1 <number_of_mappers> <number_of_reducers> <input_file>
```
Where `<input_file>` contains further files to be read from.

## **Mapper-Reducer Paradigm**
This paradigm imposes the use of two types of threads:

#### **Mappers**
Are responsible for reading data from the input files and processing it into a suitable form in order to be easily transformed into the final answer by ___Reducers___.

Each mapper outputs a list of `<number_of_reducers>` lists, each representing how many perfect powers of a given exponent have been found by the respective mapper.

#### **Reducers**
Are responsible for taking the output lists corresponding to the exponent they are assigned from the Mappers and then counting how many unique perfect powers are present.


#### **Implementation Details**
Each thread receives the following `struct` as arguments in order to be able to communicate results between them

```
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
};
```

And a wrapper `struct` that contains the threadID:
```
struct threadData
{
    int id;
    commonData *data;
};
```

In order to save as much time as possible, I precalculated perfect powers up to `INT_MAX` and stored them in an `unordered_set` to facilitate quick look-up functionality. A barrier is used to synchronise all the mappers to begin the next phase after all of the precalculations are done.

When a mapper needs to check if a number is a perfect power, it searches in the `unordered_set` and if the number is present, it therefore means that it is a perfect power and needs to be added to the corresponding output list.

Each reducer is assigned an exponent from 2 to `<number_of_reducers>` to gather from the mappers output. In order to find the number of unique numbers, I simply insert all the found perfect powers into an `unordered_set` and due to the property of unique keys, the number of elements in the set will represent the number of unique perfect powers for the given exponent.

For further details I recommend consulting the code as it is well commented and it should be straight-forward to follow along.