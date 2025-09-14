#ifndef VIDEOMETADATA_H
#define VIDEOMETADATA_H

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace std;

class VideoMetadata {
public:
    VideoMetadata();
    ~VideoMetadata();

    struct VideoData {
        uint32_t width;
        uint32_t height;
        double duration;           // Duración en segundos
        double frameRate;          // Frames por segundo
        uint32_t bitRate;          // Bitrate en kbps
        std::string codec;              // Códec de video (H.264, H.265, VP9, etc.)
        std::string audioCodec;         // Códec de audio (AAC, MP3, etc.)
        uint32_t audioBitRate;     // Bitrate de audio en kbps
        uint32_t audioSampleRate;  // Sample rate de audio en Hz
        uint8_t audioChannels;     // Número de canales de audio
        std::string container;          // Formato contenedor (MP4, AVI, MKV, etc.)
        std::string creationDate;       // Fecha de creación
        std::string title;              // Título del video
        std::string artist;             // Artista/Autor
        std::string comment;            // Comentarios
    } videoData;

    std::string filePath;

    /**
     * @brief Establece la ruta del archivo de video
     * @param filePath Ruta del archivo
     * @return string con la ruta establecida
     */
    std::string setFilePath(const std::filesystem::path &filePath);

    /**
     * @brief Extrae todos los metadatos de un archivo de video
     * @param filePath Ruta del archivo de video
     * @return string con todos los metadatos formateados
     */
    std::string extractAllMetadata(const std::filesystem::path &filePath);

    /**
     * @brief Detecta el formato del archivo de video
     * @param filePath Ruta del archivo
     * @return string con el formato detectado
     */
    std::string detectVideoFormat(const std::filesystem::path &filePath);

private:
    std::ifstream fileStream;

    // Métodos para diferentes formatos de video
    bool extractMP4Metadata();
    bool extractAVIMetadata();
    bool extractMKVMetadata();
    bool extractWEBMMetadata();
    bool extractMOVMetadata();
    bool extractFLVMetadata();
    bool extractWMVMetadata();
    
    // Método de extracción básica como fallback
    bool extractBasicVideoInfo();
    bool extractMP4BasicInfo(std::ifstream &file);
    bool extractAVIBasicInfo(std::ifstream &file);

    // Métodos auxiliares
    std::string getFileName() const;
    std::string getFileExtension() const;
    int64_t getFileSize() const;
    std::string getFormattedDuration() const;
    std::string getVideoResolution() const;
    
    // Utilidades para lectura de datos binarios
    uint32_t readUInt32BE(std::ifstream &file);
    uint32_t readUInt32LE(std::ifstream &file);
    uint16_t readUInt16BE(std::ifstream &file);
    uint16_t readUInt16LE(std::ifstream &file);
    std::string readString(std::ifstream &file, size_t length);
    
    // Métodos para parsear átomos MP4/MOV
    bool parseMP4Atoms();
    bool parseMVHDAtom(std::ifstream &file, uint32_t atomSize);
    bool parseTKHDAtom(std::ifstream &file, uint32_t atomSize);
    
    // Métodos para parsear chunks AVI
    bool parseAVIHeader();
    bool parseAVIHChunk(std::ifstream &file, uint32_t chunkSize);
    
    // Métodos para parsear elementos MKV/WEBM
    bool parseMKVElements();
    
    // Inicialización de datos
    void initializeVideoData();
};

#endif
