#ifndef METADATA_RECOVERY_ENGINE_H
#define METADATA_RECOVERY_ENGINE_H

#include <memory>
#include <string>
#include "../../include/metadata/FileMetadata.h"

class MetadataRecoveryEngine
{
private:
    std::unique_ptr<FileMetadata> fileAnalyzer;
    bool fileLoaded;
    std::string currentFilePath;

public:
    MetadataRecoveryEngine();

    bool loadFile(const std::string& filePath);
    std::string getFileType() const;
    std::string getBasicMetadata() const;
    std::string recoverDeletedMetadata();
    std::string performDeepScan();
    bool isFileLoaded() const;
};

#endif