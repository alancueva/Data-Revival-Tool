#ifndef DOCUMENT_METADATA_H
#define DOCUMENT_METADATA_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <filesystem>

using namespace std;
/**
 * Clase que representa los metadatos extraídos de un documento (PDF, DOCX, etc.).
 */
class DocumentMetadata
{
public:
    DocumentMetadata();
    ~DocumentMetadata() = default;

    /**
     * @brief Metodo para extraer los metadatos del documento.
     * @param filePath Ruta del archivo.
     * @return string con los metadatos extraídos.
     * @note Este método analiza el archivo y extrae los metadatos relevantes.
     * 
     */
    string getAllMetadata(const filesystem::path &filePath);

private:
    /**
     * @brief Ruta del archivo.
     * @note La ruta del archivo es la dirección completa donde se encuentra el archivo en el sistema de archivos.
     */
    string m_filePath;
    
    /**
     * @brief Metodo para obtener la ruta del archivo.
     * @param filePath Ruta del archivo.
     * @return string con la ruta del archivo.
     * @note El método establece la ruta del archivo y devuelve la misma.
     */
    string routeImage(const filesystem::path &filePath);
   

    /**
     * @brief se usara para almacenar los metadatos extraídos del archivo.
     * @note Los metadatos son datos que describen otros datos.
     * En este caso, se almacenan en un mapa donde la clave es el nombre del metadato
     */
    map<std::string, std::string> m_metadata;

    /**
     * @brief Se usara para almacenar los encabezados del archivo.
     */
    vector<uint8_t> m_fileHeader;

    /**
     * @brief Método para leer el encabezado del archivo.
     * @note Este método lee los primeros bytes del archivo para analizar su firma y tipo.
     */
    void readFileHeader();

    /**
     * @brief Método para analizar el archivo y extraer metadatos.
     * @return string con los metadatos extraídos.
     * @note el término "signature" indica la existencia de una firma digital.
     */
    string getFileSignature();

    /**
     * @brief Método para obtener el tamaño del archivo en bytes.
     * @note El mime_type es un estándar que describe el formato de un archivo.
     * Es una cadena de caracteres que identifica el tipo de contenido de un documento,
     * archivo o conjunto de bytes
     */
    string getMimeType();

    /**
     * @brief Método para obtener el nombre del archivo.
     * @return string con el nombre del archivo.
     */
    string getFileName() const;

    /**
     * @brief Metodo para obtener la extension de archivo.
     * @return string con la extension de archivo.
     */
    string getFileTypeExtension() const;

    /**
     * @brief Método para analizar la firma del archivo.
     * @note Este método examina la firma del archivo para identificar su tipo.
     */
    void analyzeFileSignature();

    /**
     * @brief Método para convertir un vector de bytes a una cadena hexadecimal.
     * @param bytes Vector de bytes a convertir.
     * @return string con la representación hexadecimal de los bytes.
     */
    string bytesToHex(const std::vector<uint8_t>& bytes) const ;

    /**
     * @brief Método para detectar el tipo MIME del archivo.
     * @return string con el tipo MIME del archivo.
     * @note El tipo MIME es un estándar que describe el formato de un archivo.
     * Se utiliza para identificar el tipo de contenido de un documento, archivo o conjunto de bytes.
     */
    string detectMimeType() const;
};

#endif