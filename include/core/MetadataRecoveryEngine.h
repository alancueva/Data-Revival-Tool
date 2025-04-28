#pragma once

#include <string>
#include <memory>
#include "../metadata/FileMetadata.h"

class MetadataRecoveryEngine
{
private:
    std::unique_ptr<FileMetadata> fileAnalyzer;
    bool fileLoaded;
    std::string currentFilePath;

public:
    MetadataRecoveryEngine() : fileLoaded(false) {}

    bool loadFile(const std::string& filePath);
    std::string getFileType() const;
    std::string getBasicMetadata() const;
    std::string recoverDeletedMetadata() const;
    std::string performDeepScan() const;
    bool isFileLoaded() const;
};