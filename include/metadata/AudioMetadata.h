#ifndef AUDIOMETADATA_H
#define AUDIOMETADATA_H

#include "FileMetadata.h"
#include <string>
#include <vector>

class AudioMetadata: public FileMetadata {
public:
    
    AudioMetadata(const std::string& path);

    bool analyze();

    virtual std::string getFileType() const;
    std::string extractMetadata();
    std::string recoverOverwrittenMetadata();

private:
    
    std::string audioFormat;
    std::string title;
    std::string artist;
    std::string album;
    std::string year;
    std::string genre;
    std::string filePath;
    int sampleRate;
    int bitRate;
    int channels;
    int duration;

    std::vector<unsigned char> fileHeader;
    bool isValid;
    // Métodos para análisis según tipo
    bool analyzeMP3(std::ifstream& file);
    bool analyzeWAV(std::ifstream& file);
    bool analyzeOGG(std::ifstream& file);
    bool analyzeM4A(std::ifstream& file);
};

#endif
