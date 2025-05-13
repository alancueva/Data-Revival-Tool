#ifndef FILE_ENGINE_H
#define FILE_ENGINE_H

#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <memory>
#include <future>
#include "../metadata/ImageMetadata.h"

using namespace std;

class FileEngine {
private:
    unique_ptr<ImageMetadata> imageMetadata;
    string filePath;
    string getFileType(const filesystem::path& filePath);
    string processFile(const string& filePath);
public:
    FileEngine();
    ~FileEngine();
    
    future<string> processFileAsync(const string& filePath);
};


#endif