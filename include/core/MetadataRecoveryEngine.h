#ifndef METADATA_RECOVERY_ENGINE_H
#define METADATA_RECOVERY_ENGINE_H

#include <memory>
#include <string>
#include "../../include/metadata/FileMetadata.h"
using namespace std;

class MetadataRecoveryEngine
{
private:
    unique_ptr<FileMetadata> fileAnalyzer;
    bool fileLoaded;
    string currentFilePath;

public:
    MetadataRecoveryEngine();

    bool loadFile(const string& filePath);
    string getFileType() const;
    string getBasicMetadata() const;
    string recoverDeletedMetadata();
    string performDeepScan();
    bool isFileLoaded() const;
};

#endif