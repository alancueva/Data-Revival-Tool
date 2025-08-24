#ifndef AUDIOMETADATA_H
#define AUDIOMETADATA_H

#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <cstdint> // Para uint8_t, uint32_t

class AudioMetadata
{
public:
    AudioMetadata();
    ~AudioMetadata();

    /**
     * @brief Metodo para extraer los metadatos del documento.
     * @note Este método analiza el archivo y extrae los metadatos relevantes.
     *
     */
    std::string getAllMetadata(const std::filesystem::path &filePath);

private:

    /**
     * @brief Ruta del archivo de audio.
     */
    std::filesystem::path m_filePath;

    /**
     * @brief Mapa para almacenar los metadatos del archivo de audio.
     */
    std::map<std::string, std::string> m_metadata;

    /**
     * @brief Se usara para almacenar los encabezados del archivo.
     */
    std::vector<uint8_t> m_fileHeader;

    // Métodos públicos para acceder a los metadatos
    std::string getTitle() const;
    std::string getDuration() const;
    std::string getArtist() const;
    std::string getAlbum() const;
    std::string getBitrate() const;
    std::string getSampleRate() const;
    std::string getFormat() const;
    std::string getChannels() const;

    /**
     * @brief Lee el encabezado del archivo.
     */
    void readFileHeader();

    /**
     * @brief Analiza el formato del audio.
     */
    void analyzeAudioFormat();

    /**
     * @brief Detecta el formato de audio del archivo.
     * @return El formato de audio como una cadena.
     */
    std::string detectAudioFormat();

    /**
     * @brief Metodo para extraer los metadatos de un archivo MP3.
     */
    void extractMP3Metadata();
    /**
     * @brief Método para extraer los metadatos ID3 de un archivo MP3.
     * @note Este método analiza los primeros bytes del archivo MP3 para extraer los metadatos ID3.
     */
    void extractID3Tags();

    // Funciones de utilidad
    std::string formatDuration(uint32_t seconds) const;

    /**
     * @brief Lee una cadena desde el encabezado del archivo.
     */
    std::string readString(size_t offset, size_t length) const;

    /**
     * @brief Lee un entero de 32 bits en formato big-endian desde el encabezado del archivo.
     */
    uint32_t readUInt32BE(size_t offset) const;

    /**
     * @brief Obtiene el valor de un metadato específico.
     */
    std::string getMetadataValue(const std::string& key) const;
};

#endif
