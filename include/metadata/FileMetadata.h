#ifndef FILEMETADATA_H
#define FILEMETADATA_H

#include <string>
#include <cstdint>

#include <string>
#include <filesystem>
#include <ctime>
#include <chrono>
#include <fstream>

using namespace std;

class FileMetadata
{
    public:
    /**
     * @brief Constructor de la clase FileMetadata.
     * @param path Ruta al archivo a analizar.
     * const string &path
     */
    FileMetadata();
    /*
     * @brief Destructor of the FileMetadata class.
     */
    // ~FileMetadata();


    bool analyze();
    /**
     * @brief Extrae los metadatos del archivo.
     * @return string con los metadatos extraídos.
     */
    string extractMetadata(const filesystem::path& filePath);
    string recoverOverwrittenMetadata();


    /**
     * @brief Método para analizar el archivo y extraer metadatos.
     * @return string con los metadatos extraídos.
     */
    static string getFileType(const filesystem::path& filePath);

    /**
     * Nombre del archivo, extensión y fecha de última modificación.
     * @param filePath Ruta del archivo.
     * @return Nombre del archivo.
     */
    static string getFileName(const filesystem::path& filePath);
    /**
     * @brief Obtiene la extensión del archivo a partir de su ruta.
     * @param filePath Ruta del archivo.
     * @return Extensión del archivo como string.
     */
    static string getFileExtension(const filesystem::path& filePath);
    /**
     * @brief Obtiene la fecha de última modificación del archivo.
     * @param filePath Ruta del archivo.
     * @return Fecha de última modificación como time_t.
     */
    static time_t getFileLastModified(const filesystem::path& filePath);

    // static string getFileOwnerUID();
    // static string getFileGroupGID();
    static string getFilePermissions(const filesystem::path& filePath);
    static string getFileCreationDate(const filesystem::path& filePath);

};

#endif