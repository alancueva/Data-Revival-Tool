#ifndef DOCUMENT_METADATA_H
#define DOCUMENT_METADATA_H

#include "FileMetadata.h"
#include <string>
#include <vector>

using namespace std;
/**
 * Clase que representa los metadatos extraídos de un documento (PDF, DOCX, etc.).
 * Hereda de FileMetadata y proporciona métodos específicos para el análisis y recuperación de información.
 */
class DocumentMetadata : public FileMetadata
{
public:
    DocumentMetadata(const string& path);

    bool analyze() ;
    string getFileType() const;
    string extractMetadata() ;
    string recoverOverwrittenMetadata() ;

private:
    bool analyzePDF(ifstream& file);
    bool analyzeOpenXML(ifstream& file);
    bool analyzeOldOffice(ifstream& file);
    string filePath;
    bool isValid = false;
    string documentFormat;
    string title;
    string author;
    string company;
    string creationDate;
    string lastModified;
    string application;
    string keywords;
    int pageCount;
    vector<uint8_t> fileHeader;
};

#endif 