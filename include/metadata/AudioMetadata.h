#ifndef AUDIOMETADATA_H
#define AUDIOMETADATA_H

#include "FileMetadata.h"
#include <string>
#include <vector>
using namespace std;

class AudioMetadata: public FileMetadata {
public:
    
    AudioMetadata(const string& path);

    bool analyze();

    virtual string getFileType() const;
    string extractMetadata();
    string recoverOverwrittenMetadata();

private:
    
    string audioFormat;
    string title;
    string artist;
    string album;
    string year;
    string genre;
    string filePath;
    int sampleRate;
    int bitRate;
    int channels;
    int duration;

    vector<unsigned char> fileHeader;
    bool isValid;
    // Métodos para análisis según tipo
    bool analyzeMP3(ifstream& file);
    bool analyzeWAV(ifstream& file);
    bool analyzeOGG(ifstream& file);
    bool analyzeM4A(ifstream& file);
};

#endif
