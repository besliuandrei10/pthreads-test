#include "tema1.h"

bool compareByLength(const myFile &a, const myFile &b)
{
    return a.size > b.size;
}

std::vector<myFile> readInputFiles(const std::string &filename) 
{
    // Open file containing mapper file names.
    std::ifstream f;
    f.open(filename);
    
    // Read number of entries.
    int n;
    f >> n;

    std::vector<myFile> files;
    for(auto i = 0; i < n; i++) {
        std::string mapperFile;
        f >> mapperFile;

        // Get file size.
        int file_size;

        std::ifstream in_file;
        in_file.open(mapperFile);
        in_file.seekg(0, std::ios::end);
        file_size = in_file.tellg();
        in_file.close();

        // Push file size to vector
        files.push_back(myFile());
        files[i].size = file_size;
        files[i].name = mapperFile;
    }

    std::sort(files.begin(), files.end(), compareByLength);
    return files;

}

int main(int argc, char* argv[]) {

    int M = 0; // Number of Mappers.
    int R = 0; // Number of Reducers.
    char* filepath; // Path to file.

    // Check for correct arguments.
    if (argc < 3) {
        printf("./tema1 <numar_mapperi> <numar_reduceri> <fisier_intrare>");
        return 0;
    }

    // Extract parameters.
    M = atoi(argv[0]);
    R = atoi(argv[1]);
    filepath = argv[3];

    // Remove trailing newline. Too many nightmares because of this.
    filepath[strcspn(filepath, "\n")] = 0;

    // Create sorted list of mapper files
    std::vector<myFile> files = readInputFiles(filepath);
    
    return 0;
}