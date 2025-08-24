#ifndef DOCUMENT_METADATA_H
#define DOCUMENT_METADATA_H

#include <string>
#include <vector>
#include <map>
#include <filesystem>

/**
 * Clase que representa los metadatos extraídos de un documento (PDF, DOCX, etc.).
 */
class DocumentMetadata
{
public:
    DocumentMetadata();
    ~DocumentMetadata();

    /**
     * @brief Metodo para extraer los metadatos del documento.
     * @param filePath Ruta del archivo.
     * @return string con los metadatos extraídos.
     * @note Este método analiza el archivo y extrae los metadatos relevantes.
     * 
     */
    std::string extractMetadata(const std::filesystem::path &filePath);

private:
    /**
     * @brief Ruta del archivo.
     * @note La ruta del archivo es la dirección completa donde se encuentra el archivo en el sistema de archivos.
     */
    std::filesystem::path m_filePath;

    /**
     * @brief se usara para almacenar los metadatos extraídos del archivo.
     * @note Los metadatos son datos que describen otros datos.
     * En este caso, se almacenan en un mapa donde la clave es el nombre del metadato
     */
    std::map<std::string, std::string> m_metadata;

    /**
     * @brief Se usara para almacenar los encabezados del archivo.
     */
    std::vector<uint8_t> m_fileHeader;

    /**
     * @brief Método para leer el encabezado del archivo.
     * @note Este método lee los primeros bytes del archivo para analizar su firma y tipo.
     */
    void readFileHeader();

    /**
     * @brief Método para convertir un vector de bytes a una cadena hexadecimal.
     * @param bytes Vector de bytes a convertir.
     * @return string con la representación hexadecimal de los bytes.
     */
    std::string bytesToHex(const std::vector<uint8_t>& bytes) const ;

    /**
     * @brief Método para formatear el tamaño del archivo en una cadena legible.
     * @param bytes Tamaño del archivo en bytes.
     * @return string con el tamaño formateado.
     */
    std::string formatFileSize(uint64_t bytes) const;

    /**
     * @brief Método para detectar el tipo de documento basado en su firma y contenido.
     */
    void detectDocumentType();

    /**
     * @brief Método para analizar el contenido del archivo y extraer metadatos adicionales.
     */
    void analyzeFile();

    /**
     * @brief contiene los metodos para analizar el archivo.
     */
    void parseFile();
};

#endif