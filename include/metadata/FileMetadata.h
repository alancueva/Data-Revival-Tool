#ifndef FILEMETADATA_H
#define FILEMETADATA_H

#include <string>
#include <cstdint>
using namespace std;

class FileMetadata
{
public:
    /**
     * @brief Constructor de la clase FileMetadata.
     * @param path Ruta al archivo a analizar.
     */
    FileMetadata(const string &path);
    /*
    * @brief Destructor of the FileMetadata class.
    */
    ~FileMetadata();

    bool isFileValid() const;
    string getFilePath() const;
    uint64_t getFileSize() const;
    string getBasicInfo() const;


    bool analyze();

    /**
     * @brief Método para analizar el archivo y extraer metadatos.
     * @return string con los metadatos extraídos.
     */
    virtual string getFileType() const;

    /**
     * @brief Extrae los metadatos del archivo.
     * @return string con los metadatos extraídos.
     */
    string extractMetadata();
    string recoverOverwrittenMetadata();

private:
    string filePath;
    bool isValid;
    uint64_t fileSize;
};

#endif