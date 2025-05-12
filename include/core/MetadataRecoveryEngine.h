#ifndef METADATA_RECOVERY_ENGINE_H
#define METADATA_RECOVERY_ENGINE_H

#include <memory>
#include <string>
#include "../../include/metadata/FileMetadata.h"
using namespace std;

class FileMetadata;
class MetadataRecoveryEngine
{
private:
    unique_ptr<FileMetadata> fileAnalyzer;
    FileMetadata *fileMetadata;
public:
    MetadataRecoveryEngine();
    ~MetadataRecoveryEngine();

    string recoverMetadata(const filesystem::path &filePath);
};

#endif