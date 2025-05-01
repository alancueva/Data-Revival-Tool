#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

#include "FileMetadata.h"
#include <string>
#include <vector>
using namespace std;

class ImageMetadata : public FileMetadata {
public:
    ImageMetadata(const string& path);
    
    bool analyze() ;
    string getFileType() const ;
    string extractMetadata() ;
    string recoverOverwrittenMetadata() ;

private:
    string imageFormat;
    vector<unsigned char> fileHeader;

    string filePath;
    bool isValid = false;

    int width;
    int height;
    int colorDepth;
    
    string creationDate;
    string camera;
    string software;
    
    bool hasGPS = false;
    string gpsLatitude;
    string gpsLongitude;

    bool analyzeJPEG(ifstream& file);
    bool analyzePNG(ifstream& file);
    bool analyzeTIFF(ifstream& file);
    bool analyzeBMP(ifstream& file);
    bool analyzeGIF(ifstream& file);
};

#endif