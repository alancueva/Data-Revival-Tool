#ifndef AUDIOMETADATA_H
#define AUDIOMETADATA_H

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
using namespace std;

class AudioMetadata
{
public:
    AudioMetadata();
    ~AudioMetadata();

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
     * @brief Ruta del archivo de audio.
     */
    string filePath;

    /**
     * @brief Mapa para almacenar los metadatos del archivo de audio.
     */
    map<string, string> m_metadata;

    /**
     * @brief Se usara para almacenar los encabezados del archivo.
     */
    vector<uint8_t> m_fileHeader;

    /**
     * @brief Metodo para obtener la ruta del archivo de audio.
     * @param filePath Ruta del archivo de audio.
     * @return string con la ruta del archivo de audio.
     */
    string routeAudio(const filesystem::path &filePath);

    string getTitle() const;
    string getDuration() const;
    string getArtist() const;
    string getAlbum() const;
    string getBitrate() const;
    string getSampleRate() const;
    string getFormat() const;
    string getChannels() const;

    void readFileHeader();

    string detectAudioFormat();

    void analyzeAudioFormat();

    string formatDuration(uint32_t seconds) const;

    /**
     * @brief Metodo para extraer los metadatos de un archivo MP3.
     */
    void extractMP3Metadata();

    /**
     * @brief Método para extraer los metadatos ID3 de un archivo MP3.
     * @note Este método analiza los primeros bytes del archivo MP3 para extraer los metadatos ID3.
     */
    void extractID3Tags();
    void extractWAVMetadata();

    string readString(size_t offset, size_t length) const;
    uint32_t readUInt32BE(size_t offset) const ;

};

#endif
