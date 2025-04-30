#ifndef FILEMETADATA_H
#define FILEMETADATA_H

#include <string>
#include <cstdint>

class FileMetadata {
public:
    
    FileMetadata(const std::string& path);

    bool isFileValid() const;
    std::string getFilePath() const;
    uint64_t getFileSize() const;
    std::string getBasicInfo() const;

    virtual ~FileMetadata() = default;

     bool analyze();
     std::string getFileType() const ;
     std::string extractMetadata();
     std::string recoverOverwrittenMetadata();


private:
    std::string filePath;
    bool isValid;
    uint64_t fileSize;

};

#endif 