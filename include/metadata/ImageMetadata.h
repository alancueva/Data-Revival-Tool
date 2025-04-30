#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

#include "FileMetadata.h"
#include <string>
#include <vector>

class ImageMetadata : public FileMetadata {
public:
    ImageMetadata(const std::string& path);
    
    bool analyze() ;
    std::string getFileType() const ;
    std::string extractMetadata() ;
    std::string recoverOverwrittenMetadata() ;

private:
    std::string imageFormat;
    std::vector<unsigned char> fileHeader;

    std::string filePath;
    bool isValid = false;

    int width;
    int height;
    int colorDepth;
    
    std::string creationDate;
    std::string camera;
    std::string software;
    
    bool hasGPS = false;
    std::string gpsLatitude;
    std::string gpsLongitude;

    bool analyzeJPEG(std::ifstream& file);
    bool analyzePNG(std::ifstream& file);
    bool analyzeTIFF(std::ifstream& file);
    bool analyzeBMP(std::ifstream& file);
    bool analyzeGIF(std::ifstream& file);
};

#endif