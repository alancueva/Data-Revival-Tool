#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <ctime>


// FileMetadata.h
// Este archivo define la clase abstracta FileMetadata, que sirve como interfaz base 
// para manejar metadatos de archivos en C++.
// La clase almacena información básica como la ruta del archivo, el encabezado, 
// el tipo y formato del archivo, y si es válido.
// Se utiliza para analizar archivos, extraer y recuperar metadatos, y obtener información 
// relevante del archivo.
// Las clases derivadas deben implementar los métodos virtuales puros para adaptarse a diferentes tipos y formatos de archivos.
class FileMetadata{

protected:
    std::string filepath;
    std::vector<uint8_t> fileHeader;
    bool isValid;
    std::string fileType;
    std::string fileFormat;

public:
    FileMetadata(const std::string& path);
    virtual ~FileMetadata() = default;
    virtual bool analyze() = 0;
    virtual std::string getFileType() const = 0;
    virtual std::string getFileFormat() const;
    virtual std::string extractMetadata() = 0;
    virtual std::string recoverOverwrittenMetadata() = 0;
    virtual std::string getFilePath() const;
    virtual std::string getFileName() const;
    virtual std::string getFileExtension() const;
    virtual std::string getFileSize() const;
    virtual std::chrono::time_point<std::chrono::system_clock> getCreationDate() const;
    virtual int getEditingToolsCount() const;
    virtual std::string getEditingTools() const;
    virtual std::string getLastModificationDate() const;
    virtual std::string getLastModificationDate(const std::string& filePath) const;
    virtual int getModificationCount() const;
    virtual std::string getLastAccessDate() const;
    virtual int getAnalyzedSectors() const;
    virtual long getResidualMetadataBlocks() const;
    virtual bool hasModificationHistory() const;
    virtual std::string getRecoveredBytesSize() const;

    bool isFileValid() const;
    std::vector<uint8_t> getFileHeader() const;
    void setFileHeader(const std::vector<uint8_t>& header);
    std::string getFilePath() const;
};
