#ifndef FILEMETADATA_H
#define FILEMETADATA_H

#include <string>
#include <cstdint>

class FileMetadata {
public:
    
    explicit FileMetadata(const std::string& path);

    bool isFileValid() const;
    std::string getFilePath() const;
    uint64_t getFileSize() const;
    std::string getBasicInfo() const;

private:
    std::string filePath;
    bool isValid;
    uint64_t fileSize;

    std::string getFileType() const;
};

#endif 