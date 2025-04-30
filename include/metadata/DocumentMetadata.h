#ifndef DOCUMENT_METADATA_H
#define DOCUMENT_METADATA_H

#include "FileMetadata.h"
#include <string>
#include <vector>

/**
 * Clase que representa los metadatos extraídos de un documento (PDF, DOCX, etc.).
 * Hereda de FileMetadata y proporciona métodos específicos para el análisis y recuperación de información.
 */
class DocumentMetadata : public FileMetadata
{
public:
    DocumentMetadata(const std::string& path);

    bool analyze() ;
    std::string getFileType() const ;
    std::string extractMetadata() ;
    std::string recoverOverwrittenMetadata() ;

private:
    bool analyzePDF(std::ifstream& file);
    bool analyzeOpenXML(std::ifstream& file);
    bool analyzeOldOffice(std::ifstream& file);
    std::string filePath;
    bool isValid = false;
    std::string documentFormat;
    std::string title;
    std::string author;
    std::string company;
    std::string creationDate;
    std::string lastModified;
    std::string application;
    std::string keywords;
    int pageCount;
    std::vector<uint8_t> fileHeader;
};

#endif 