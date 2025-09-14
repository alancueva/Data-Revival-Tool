#include "../../include/metadata/VideoMetadata.h"
#include "../../include/utils/SizeFormatter.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <filesystem>
#include <algorithm>

VideoMetadata::VideoMetadata() {
    initializeVideoData();
    filePath = "";
}

VideoMetadata::~VideoMetadata() {
}

void VideoMetadata::initializeVideoData() {
    videoData.width = 0;
    videoData.height = 0;
    videoData.duration = 0.0;
    videoData.frameRate = 0.0;
    videoData.bitRate = 0;
    videoData.codec = "Unknown";
    videoData.audioCodec = "Unknown";
    videoData.audioBitRate = 0;
    videoData.audioSampleRate = 0;
    videoData.audioChannels = 0;
    videoData.container = "Unknown";
    videoData.creationDate = "Unknown";
    videoData.title = "Unknown";
    videoData.artist = "Unknown";
    videoData.comment = "Unknown";
}

std::string VideoMetadata::setFilePath(const std::filesystem::path &filePath) {
    this->filePath = filePath.string();
    return this->filePath;
}

std::string VideoMetadata::detectVideoFormat(const std::filesystem::path &filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return "Error: No se pudo abrir el archivo";
    }

    char header[32] = {};
    file.read(header, sizeof(header));

    // MP4/MOV - buscar ftyp atom
    if (memcmp(header + 4, "ftyp", 4) == 0) {
        // Verificar subtipos
        if (memcmp(header + 8, "isom", 4) == 0 || memcmp(header + 8, "mp41", 4) == 0 || 
            memcmp(header + 8, "mp42", 4) == 0 || memcmp(header + 8, "avc1", 4) == 0) {
            return "MP4";
        } else if (memcmp(header + 8, "qt  ", 4) == 0) {
            return "MOV";
        }
        return "MP4/MOV";
    }

    // AVI - RIFF header con AVI
    if (memcmp(header, "RIFF", 4) == 0 && memcmp(header + 8, "AVI ", 4) == 0) {
        return "AVI";
    }

    // MKV/WEBM - EBML header
    if (memcmp(header, "\x1A\x45\xDF\xA3", 4) == 0) {
        // Leer más para determinar si es MKV o WEBM
        file.seekg(0);
        char buffer[1024];
        file.read(buffer, sizeof(buffer));
        std::string content(buffer, sizeof(buffer));
        if (content.find("webm") != std::string::npos) {
            return "WEBM";
        }
        return "MKV";
    }

    // FLV
    if (memcmp(header, "FLV", 3) == 0) {
        return "FLV";
    }

    // WMV/ASF
    if (memcmp(header, "\x30\x26\xB2\x75\x8E\x66\xCF\x11\xA6\xD9\x00\xAA\x00\x62\xCE\x6C", 16) == 0) {
        return "WMV/ASF";
    }

    // 3GP
    if (memcmp(header + 4, "ftyp3gp", 7) == 0) {
        return "3GP";
    }

    return "Formato desconocido";
}

std::string VideoMetadata::extractAllMetadata(const std::filesystem::path &filePath) {
    setFilePath(filePath);
    initializeVideoData();
    
    std::stringstream metadata;
    
    // Validar que el archivo existe y es accesible
    if (!std::filesystem::exists(filePath)) {
        metadata << "ERROR: El archivo no existe: " << filePath << "\n";
        return metadata.str();
    }
    
    if (!std::filesystem::is_regular_file(filePath)) {
        metadata << "ERROR: La ruta no apunta a un archivo regular: " << filePath << "\n";
        return metadata.str();
    }
    
    // Verificar permisos de lectura
    std::ifstream testFile(filePath, std::ios::binary);
    if (!testFile.is_open()) {
        metadata << "ERROR: No se puede abrir el archivo para lectura: " << filePath << "\n";
        metadata << "Posibles causas:\n";
        metadata << "- Sin permisos de lectura\n";
        metadata << "- Archivo en uso por otra aplicación\n";
        metadata << "- Caracteres especiales en la ruta (acentos, ñ, etc.)\n";
        metadata << "- Archivo corrupto o bloqueado\n";
        return metadata.str();
    }
    testFile.close();
    
    std::string format = detectVideoFormat(filePath);
    videoData.container = format;

    metadata << "== METADATOS DE VIDEO ==\n\n"
             << "Nombre del archivo: " << getFileName() << "\n"
             << "Tamaño del archivo: " << format_size(getFileSize()) << "\n"
             << "Formato/Contenedor: " << format << "\n"
             << "Extensión: " << getFileExtension() << "\n\n";

    // Intentar extraer metadatos específicos según el formato
    bool extracted = false;
    if (format == "MP4" || format == "MOV" || format == "MP4/MOV") {
        extracted = extractMP4Metadata();
    } else if (format == "AVI") {
        extracted = extractAVIMetadata();
    } else if (format == "MKV") {
        extracted = extractMKVMetadata();
    } else if (format == "WEBM") {
        extracted = extractWEBMMetadata();
    } else if (format == "FLV") {
        extracted = extractFLVMetadata();
    } else if (format == "WMV/ASF") {
        extracted = extractWMVMetadata();
    }
    
    // Si no se pudo extraer con métodos específicos, intentar extracción básica
    if (!extracted) {
        extractBasicVideoInfo();
    }

    if (extracted) {
        metadata << "=== INFORMACIÓN TÉCNICA ===\n"
                 << "Resolución: " << getVideoResolution() << "\n"
                 << "Duración: " << getFormattedDuration() << "\n"
                 << "Frame Rate: " << std::fixed << std::setprecision(2) << videoData.frameRate << " fps\n"
                 << "Bitrate Video: " << videoData.bitRate << " kbps\n"
                 << "Códec Video: " << videoData.codec << "\n\n"
                 << "=== INFORMACIÓN DE AUDIO ===\n"
                 << "Códec Audio: " << videoData.audioCodec << "\n"
                 << "Bitrate Audio: " << videoData.audioBitRate << " kbps\n"
                 << "Sample Rate: " << videoData.audioSampleRate << " Hz\n"
                 << "Canales: " << static_cast<int>(videoData.audioChannels) << "\n\n"
                 << "=== METADATOS ADICIONALES ===\n"
                 << "Título: " << videoData.title << "\n"
                 << "Artista/Autor: " << videoData.artist << "\n"
                 << "Fecha de creación: " << videoData.creationDate << "\n"
                 << "Comentarios: " << videoData.comment << "\n";
    } else {
        metadata << "No se pudieron extraer metadatos específicos para este formato.\n"
                 << "El archivo parece ser válido pero el análisis detallado no está disponible.\n";
    }

    return metadata.str();
}

bool VideoMetadata::extractMP4Metadata() {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return false;

    try {
        return parseMP4Atoms();
    } catch (const std::exception &e) {
        std::cerr << "Error parsing MP4: " << e.what() << std::endl;
        return false;
    }
}

bool VideoMetadata::parseMP4Atoms() {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return false;

    while (!file.eof()) {
        uint32_t atomSize = readUInt32BE(file);
        if (atomSize == 0) break;

        char atomType[5] = {0};
        file.read(atomType, 4);

        if (strcmp(atomType, "mvhd") == 0) {
            parseMVHDAtom(file, atomSize - 8);
        } else if (strcmp(atomType, "tkhd") == 0) {
            parseTKHDAtom(file, atomSize - 8);
        } else {
            // Saltar atom desconocido
            file.seekg(atomSize - 8, std::ios::cur);
        }

        if (file.tellg() >= static_cast<std::streampos>(getFileSize())) break;
    }

    // Establecer valores por defecto si no se encontraron
    if (videoData.codec == "Unknown") {
        videoData.codec = "H.264 (estimado)";
    }
    if (videoData.audioCodec == "Unknown") {
        videoData.audioCodec = "AAC (estimado)";
    }

    return true;
}

bool VideoMetadata::parseMVHDAtom(std::ifstream &file, uint32_t atomSize) {
    if (atomSize < 24) return false;

    uint8_t version;
    file.read(reinterpret_cast<char*>(&version), 1);
    file.seekg(3, std::ios::cur); // flags

    if (version == 0) {
        file.seekg(8, std::ios::cur); // creation/modification time
        uint32_t timeScale = readUInt32BE(file);
        uint32_t duration = readUInt32BE(file);
        
        if (timeScale > 0) {
            videoData.duration = static_cast<double>(duration) / timeScale;
        }
    } else {
        file.seekg(16, std::ios::cur); // 64-bit times
        uint32_t timeScale = readUInt32BE(file);
        uint64_t duration = (static_cast<uint64_t>(readUInt32BE(file)) << 32) | readUInt32BE(file);
        
        if (timeScale > 0) {
            videoData.duration = static_cast<double>(duration) / timeScale;
        }
    }

    return true;
}

bool VideoMetadata::parseTKHDAtom(std::ifstream &file, uint32_t atomSize) {
    if (atomSize < 32) return false;

    uint8_t version;
    file.read(reinterpret_cast<char*>(&version), 1);
    file.seekg(3, std::ios::cur); // flags

    if (version == 0) {
        file.seekg(32, std::ios::cur); // skip to matrix
        file.seekg(36, std::ios::cur); // skip matrix
        uint32_t width = readUInt32BE(file) >> 16;  // fixed point
        uint32_t height = readUInt32BE(file) >> 16; // fixed point
        
        videoData.width = width;
        videoData.height = height;
    }

    return true;
}

bool VideoMetadata::extractAVIMetadata() {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return false;

    try {
        return parseAVIHeader();
    } catch (const std::exception &e) {
        std::cerr << "Error parsing AVI: " << e.what() << std::endl;
        return false;
    }
}

bool VideoMetadata::parseAVIHeader() {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return false;

    // Buscar chunk 'avih'
    file.seekg(12); // Saltar RIFF header
    
    while (!file.eof()) {
        char chunkId[5] = {0};
        file.read(chunkId, 4);
        uint32_t chunkSize = readUInt32LE(file);

        if (strcmp(chunkId, "avih") == 0) {
            return parseAVIHChunk(file, chunkSize);
        } else {
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    return false;
}

bool VideoMetadata::parseAVIHChunk(std::ifstream &file, uint32_t chunkSize) {
    if (chunkSize < 56) return false;

    uint32_t microSecPerFrame = readUInt32LE(file);
    file.seekg(8, std::ios::cur); // skip max bytes per sec and padding
    uint32_t flags = readUInt32LE(file);
    uint32_t totalFrames = readUInt32LE(file);
    file.seekg(4, std::ios::cur); // skip initial frames
    uint32_t streams = readUInt32LE(file);
    file.seekg(4, std::ios::cur); // skip suggested buffer size
    uint32_t width = readUInt32LE(file);
    uint32_t height = readUInt32LE(file);

    videoData.width = width;
    videoData.height = height;
    
    if (microSecPerFrame > 0) {
        videoData.frameRate = 1000000.0 / microSecPerFrame;
        videoData.duration = totalFrames / videoData.frameRate;
    }

    videoData.codec = "AVI Codec (sin especificar)";
    videoData.audioCodec = "PCM/MP3 (estimado)";

    return true;
}

bool VideoMetadata::extractMKVMetadata() {
    // Implementación básica para MKV
    videoData.codec = "VP8/VP9/H.264 (MKV)";
    videoData.audioCodec = "Vorbis/Opus (MKV)";
    return true;
}

bool VideoMetadata::extractWEBMMetadata() {
    // Implementación básica para WEBM
    videoData.codec = "VP8/VP9";
    videoData.audioCodec = "Vorbis/Opus";
    return true;
}

bool VideoMetadata::extractMOVMetadata() {
    // MOV usa la misma estructura que MP4
    return extractMP4Metadata();
}

bool VideoMetadata::extractFLVMetadata() {
    // Implementación básica para FLV
    videoData.codec = "H.264/VP6";
    videoData.audioCodec = "AAC/MP3";
    return true;
}

bool VideoMetadata::extractWMVMetadata() {
    // Implementación básica para WMV
    videoData.codec = "WMV/VC-1";
    videoData.audioCodec = "WMA";
    return true;
}

bool VideoMetadata::extractBasicVideoInfo() {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return false;

    try {
        // Leer header del archivo para determinar formato y extraer info básica
        char header[64] = {};
        file.read(header, sizeof(header));
        
        // MP4/MOV - buscar dimensiones en ftyp y moov atoms
        if (memcmp(header + 4, "ftyp", 4) == 0) {
            return extractMP4BasicInfo(file);
        }
        
        // AVI - buscar en RIFF header
        if (memcmp(header, "RIFF", 4) == 0 && memcmp(header + 8, "AVI ", 4) == 0) {
            return extractAVIBasicInfo(file);
        }
        
        // Para otros formatos, establecer valores estimados basados en el tamaño del archivo
        int64_t fileSize = getFileSize();
        if (fileSize > 0) {
            // Estimación muy básica basada en el tamaño del archivo
            if (fileSize < 50 * 1024 * 1024) { // < 50MB
                videoData.width = 640;
                videoData.height = 480;
                videoData.duration = 300; // 5 minutos estimados
            } else if (fileSize < 200 * 1024 * 1024) { // < 200MB
                videoData.width = 1280;
                videoData.height = 720;
                videoData.duration = 1800; // 30 minutos estimados
            } else { // > 200MB
                videoData.width = 1920;
                videoData.height = 1080;
                videoData.duration = 3600; // 1 hora estimada
            }
            
            videoData.frameRate = 30.0;
            videoData.bitRate = static_cast<uint32_t>((fileSize * 8) / (videoData.duration * 1000)); // kbps estimado
            
            return true;
        }
        
    } catch (const std::exception &e) {
        std::cerr << "Error en extracción básica: " << e.what() << std::endl;
    }
    
    return false;
}

bool VideoMetadata::extractMP4BasicInfo(std::ifstream &file) {
    file.seekg(0);
    
    // Buscar atoms moov/mvhd para duración y tkhd para dimensiones
    while (!file.eof()) {
        uint32_t atomSize = readUInt32BE(file);
        if (atomSize == 0 || atomSize > 1000000) break; // Evitar atoms muy grandes
        
        char atomType[5] = {0};
        file.read(atomType, 4);
        
        if (strcmp(atomType, "moov") == 0) {
            // Dentro del atom moov, buscar mvhd y tkhd
            long moovStart = file.tellg();
            long moovEnd = moovStart + atomSize - 8;
            
            while (file.tellg() < moovEnd && !file.eof()) {
                uint32_t subAtomSize = readUInt32BE(file);
                if (subAtomSize == 0) break;
                
                char subAtomType[5] = {0};
                file.read(subAtomType, 4);
                
                if (strcmp(subAtomType, "mvhd") == 0) {
                    parseMVHDAtom(file, subAtomSize - 8);
                } else if (strcmp(subAtomType, "tkhd") == 0) {
                    parseTKHDAtom(file, subAtomSize - 8);
                } else {
                    file.seekg(subAtomSize - 8, std::ios::cur);
                }
            }
            break;
        } else {
            file.seekg(atomSize - 8, std::ios::cur);
        }
        
        if (file.tellg() >= getFileSize()) break;
    }
    
    // Establecer valores por defecto si no se encontraron
    if (videoData.codec == "Unknown") {
        videoData.codec = "H.264";
    }
    if (videoData.audioCodec == "Unknown") {
        videoData.audioCodec = "AAC";
    }
    if (videoData.frameRate == 0.0) {
        videoData.frameRate = 30.0;
    }
    
    return (videoData.width > 0 && videoData.height > 0) || videoData.duration > 0;
}

bool VideoMetadata::extractAVIBasicInfo(std::ifstream &file) {
    file.seekg(12); // Saltar RIFF header
    
    while (!file.eof()) {
        char chunkId[5] = {0};
        file.read(chunkId, 4);
        uint32_t chunkSize = readUInt32LE(file);
        
        if (strcmp(chunkId, "LIST") == 0) {
            char listType[5] = {0};
            file.read(listType, 4);
            
            if (strcmp(listType, "hdrl") == 0) {
                // Buscar avih chunk dentro de hdrl
                std::streampos currentPos = file.tellg();
                long listEnd = static_cast<long>(currentPos) + chunkSize - 4;
                
                while (static_cast<long>(file.tellg()) < listEnd && !file.eof()) {
                    char subChunkId[5] = {0};
                    file.read(subChunkId, 4);
                    uint32_t subChunkSize = readUInt32LE(file);
                    
                    if (strcmp(subChunkId, "avih") == 0) {
                        parseAVIHChunk(file, subChunkSize);
                        break;
                    } else {
                        file.seekg(subChunkSize, std::ios::cur);
                    }
                }
                break;
            } else {
                file.seekg(chunkSize - 4, std::ios::cur);
            }
        } else {
            file.seekg(chunkSize, std::ios::cur);
        }
    }
    
    if (videoData.codec == "Unknown") {
        videoData.codec = "XVID/DivX";
    }
    if (videoData.audioCodec == "Unknown") {
        videoData.audioCodec = "MP3/PCM";
    }
    
    return (videoData.width > 0 && videoData.height > 0) || videoData.duration > 0;
}

// Métodos auxiliares
std::string VideoMetadata::getFileName() const {
    return std::filesystem::path(filePath).filename().string();
}

std::string VideoMetadata::getFileExtension() const {
    return std::filesystem::path(filePath).extension().string();
}

int64_t VideoMetadata::getFileSize() const {
    std::ifstream fs(filePath, std::ios::binary | std::ios::ate);
    if (!fs) return -1;
    return fs.tellg();
}

std::string VideoMetadata::getFormattedDuration() const {
    if (videoData.duration <= 0) return "Unknown";

    int hours = static_cast<int>(videoData.duration) / 3600;
    int minutes = (static_cast<int>(videoData.duration) % 3600) / 60;
    int seconds = static_cast<int>(videoData.duration) % 60;

    std::ostringstream out;
    if (hours > 0) {
        out << hours << "h " << minutes << "m " << seconds << "s";
    } else if (minutes > 0) {
        out << minutes << "m " << seconds << "s";
    } else {
        out << seconds << "s";
    }
    return out.str();
}

std::string VideoMetadata::getVideoResolution() const {
    if (videoData.width == 0 || videoData.height == 0) {
        return "Unknown";
    }
    return std::to_string(videoData.width) + "x" + std::to_string(videoData.height);
}

// Utilidades para lectura binaria
uint32_t VideoMetadata::readUInt32BE(std::ifstream &file) {
    uint8_t bytes[4];
    file.read(reinterpret_cast<char*>(bytes), 4);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

uint32_t VideoMetadata::readUInt32LE(std::ifstream &file) {
    uint8_t bytes[4];
    file.read(reinterpret_cast<char*>(bytes), 4);
    return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

uint16_t VideoMetadata::readUInt16BE(std::ifstream &file) {
    uint8_t bytes[2];
    file.read(reinterpret_cast<char*>(bytes), 2);
    return (bytes[0] << 8) | bytes[1];
}

uint16_t VideoMetadata::readUInt16LE(std::ifstream &file) {
    uint8_t bytes[2];
    file.read(reinterpret_cast<char*>(bytes), 2);
    return bytes[0] | (bytes[1] << 8);
}

std::string VideoMetadata::readString(std::ifstream &file, size_t length) {
    std::vector<char> buffer(length);
    file.read(buffer.data(), length);
    return std::string(buffer.begin(), buffer.end());
}
