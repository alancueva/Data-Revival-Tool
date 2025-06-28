#ifndef METADATA_RECOVERY_ENGINE_H
#define METADATA_RECOVERY_ENGINE_H

#include <memory>
#include <string>
#include <filesystem>
#include <future>
#include "FileEngine.h"

using namespace std;

class MetadataRecoveryEngine
{
private: 
    unique_ptr<FileEngine> fileEngine;
    string filePath;
    string routeImage(const filesystem::path &filePath);

    public:
    MetadataRecoveryEngine();
    ~MetadataRecoveryEngine();
    
    string recoverMetadata(const filesystem::path &filePath);
    
    string deleteMetadata(const std::string &filepath);
};

#endif