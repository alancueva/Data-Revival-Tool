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
     */
    FileMetadata();
    /**
     * @brief Destructor de la clase FileMetadata.
     */
    ~FileMetadata();

    /**
     * --------------------------EXTRACION DE METADATA BASICO--------------------------
     * @brief Extrae los metadatos del archivo.
     * @param filePath Ruta del archivo.
     * @return string con los metadatos extraídos.
     */
    string extractMetadata(const filesystem::path &filePath);

    /**
     * @brief Método para analizar el archivo y extraer metadatos.
     * @return string con los metadatos extraídos.
     */
    static string getFileType(const filesystem::path &filePath);

    /**
     * Nombre del archivo, extensión y fecha de última modificación.
     * @param filePath Ruta del archivo.
     * @return Nombre del archivo.
     */

    static string getFileName(const filesystem::path &filePath);
    /**
     * @brief Obtiene la extensión del archivo a partir de su ruta.
     * @param filePath Ruta del archivo.
     * @return Extensión del archivo como string.
     */

    static string getFileExtension(const filesystem::path &filePath);
    /**
     * @brief Obtiene la fecha de última modificación del archivo.
     * @param filePath Ruta del archivo.
     * @return Fecha de última modificación como time_t.
     */

    static time_t getFileLastModified(const filesystem::path &filePath);

    /**
     * @brief Obtiene la fecha de última modificación del archivo.
     * @param filePath Ruta del archivo.
     * @return Fecha de última modificación como string.
     */

    static string getFileLastModifiedstr(const filesystem::path &filePath);
    /**
     * @brief Obtiene el tamaño del archivo.
     * @param filePath Ruta del archivo.
     * @return Tamaño del archivo como string.
     * La unidad de medida se elige automáticamente (bytes, KB, MB, GB, TB).
     */

    static string getFileSize(const filesystem::path &filePath);
    /**
     * @brief Recupera metadatos sobrescritos.
     * @return string con los metadatos recuperados.
     */

    static string getFilePermissions(const filesystem::path &filePath);
    /**
     * @brief Obtiene la fecha de creación del archivo.
     * @param filePath Ruta del archivo.
     * @return Fecha de creación como string.
     */
    
    static string getFileCreationDate(const filesystem::path &filePath);
};

#endif