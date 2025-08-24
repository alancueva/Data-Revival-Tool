#include "../../include/metadata/ImageMetadata.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <filesystem>

ImageMetadata::ImageMetadata() {
    imageData.width = 0;
    imageData.height = 0;
    imageData.bitDepth = 0;
    imageData.colorType = 0;
    imageData.compression = 0;
    imageData.filter = 0;
    imageData.interlace = 0;
    imageData.gamma = 0.0;
    imageData.pixelsPerUnitX = 0;
    imageData.pixelsPerUnitY = 0;
    imageData.pixelUnits = "unknown";
    imageData.srgbRendering = "Unknown";

    filePath = "";
}

ImageMetadata::~ImageMetadata() {
}

std::string ImageMetadata::setFilePath(const std::filesystem::path &filePath) {
    this->filePath = filePath.string();
    return this->filePath;
}


/**
 * @brief Método para analizar el archivo y extraer metadatos.
 * @return string con los metadatos extraídos.
 */
std::string ImageMetadata::extractMetadataImg(const std::filesystem::path &filePath) {
    setFilePath(filePath);
    std::stringstream metadata;
    metadata << "== METADATOS DE IMAGEN ==\n\n"
             << "Nombre del archivo: " << getFileName() << "\n"
             << "Tamaño del archivo: " << getFormattedFileSize() << "\n"
             << "Tipo de archivo: " << getFileType() << "\n"
             << "Extensión: " << getFileTypeExtension() << "\n"
             << "Profundidad de bits: " << static_cast<int>(getBitDepth()) << "\n"
             << "Tipo de color: " << getColorType() << "\n"
             << "Dimensiones de la Imagen: " << getDimensionImage() << "\n"
             << "Unidades de píxel: " << getPixelUnits() << "\n";
    return metadata.str();
}


std::string ImageMetadata::getPixelUnits() const {
    return imageData.pixelUnits;
}

double ImageMetadata::getMegapixels() const {
    return static_cast<double>(imageData.width * imageData.height) / 1000000.0;
}

std::string ImageMetadata::getFileType() const {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "No se pudo abrir el archivo: " << filePath << "\n";
        return "";
    }
    char header[12] = {} ;
    file.read(header, sizeof(header));

    // PNG
    if (memcmp(header, "\x89PNG\r\n\x1a\n", 8) == 0) {
        return "PNG";
    }
    // JPG
    else if (memcmp(header, "\xFF\xD8\xFF", 3) == 0) {
        return "JPG";
    }
    // GIF
    else if (memcmp(header, "GIF87a", 6) == 0 || memcmp(header, "GIF89a", 6) == 0) {
        return "GIF";
    }
    // BMP
    else if (memcmp(header, "BM", 2) == 0) {
        return "BMP";
    }
    // TIFF
    else if (memcmp(header, "II*\x00", 4) == 0 || memcmp(header, "MM\x00*", 4) == 0) {
        return "TIFF";
    }
    // WEBP
    else if (memcmp(header, "RIFF", 4) == 0 && memcmp(header + 8, "WEBP", 4) == 0) {
        return "WEBP";
    }

    return "Desconocido";
}


std::string ImageMetadata::getDimensionImage() const {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "No se pudo abrir el archivo: " << filePath << "\n";
        return "";
    }

    char header[32] = {};
    file.read(header, sizeof(header));

    // ----------------------------
    // PNG: firma + IHDR
    if (memcmp(header, "\x89PNG\r\n\x1a\n", 8) == 0) {
        uint32_t width = (uint8_t)header[16] << 24 | (uint8_t)header[17] << 16 |
                         (uint8_t)header[18] << 8 | (uint8_t)header[19];
        uint32_t height = (uint8_t)header[20] << 24 | (uint8_t)header[21] << 16 |
                          (uint8_t)header[22] << 8 | (uint8_t)header[23];
        return std::to_string(width) + "x" + std::to_string(height);
    }

    // ----------------------------
    // BMP: firma "BM"
    if (memcmp(header, "BM", 2) == 0) {
        // Lectura segura para Little Endian (formato BMP)
        int32_t width = (uint8_t)header[18] | ((uint8_t)header[19] << 8) | ((uint8_t)header[20] << 16) | ((uint8_t)header[21] << 24);
        int32_t height = (uint8_t)header[22] | ((uint8_t)header[23] << 8) | ((uint8_t)header[24] << 16) | ((uint8_t)header[25] << 24);
        return std::to_string(width) + "x" + std::to_string(height);
    }

    // ----------------------------
    // GIF: firma "GIF87a" o "GIF89a"
    if (memcmp(header, "GIF87a", 6) == 0 || memcmp(header, "GIF89a", 6) == 0) {
        // Lectura segura para Little Endian (formato GIF)
        uint16_t width = (uint8_t)header[6] | ((uint8_t)header[7] << 8);
        uint16_t height = (uint8_t)header[8] | ((uint8_t)header[9] << 8);
        return std::to_string(width) + "x" + std::to_string(height);
    }

    // ----------------------------
    // JPG: hay que buscar el marcador SOFn (no está al inicio)
    if ((uint8_t)header[0] == 0xFF && (uint8_t)header[1] == 0xD8) {
        file.seekg(2);
        while (!file.eof()) {
            uint8_t marker[2];
            file.read((char*)marker, 2);
            if (marker[0] != 0xFF) break;

            if (marker[1] >= 0xC0 && marker[1] <= 0xC3) { // SOF0 - SOF3
                char data[7];
                file.read(data, 7);
                uint16_t height = ((uint8_t)data[1] << 8) | (uint8_t)data[2];
                uint16_t width = ((uint8_t)data[3] << 8) | (uint8_t)data[4];
                return std::to_string(width) + "x" + std::to_string(height);
            } else {
                char lenBytes[2];
                file.read(lenBytes, 2);
                uint16_t blockLength = ((uint8_t)lenBytes[0] << 8) | (uint8_t)lenBytes[1];
                file.seekg(blockLength - 2, std::ios::cur);
            }
        }

        std::cout << "[JPG] No se pudo leer dimensiones.\n";
        return "";
    }

    std::cout << "Formato desconocido o no soportado.\n";
    return "";
}



// Método para obtener el tamaño del archivo en bytes
int64_t ImageMetadata::getFileSize() const {
    std::ifstream fs(filePath, std::ios::binary | std::ios::ate);
    if (!fs) return -1;
    return fs.tellg();
}

// Método para formatear el tamaño del archivo como string (KB, MB, etc.)
std::string ImageMetadata::getFormattedFileSize() const {
    int64_t size = getFileSize();
    if (size < 0) return "Unknown";

    const char* units[] = { "B", "KB", "MB", "GB", "TB" };
    int unitIndex = 0;
    double formattedSize = static_cast<double>(size);

    while (formattedSize >= 1024.0 && unitIndex < 4) {
        formattedSize /= 1024.0;
        ++unitIndex;
    }

    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << formattedSize << " " << units[unitIndex];
    return out.str();
}

std::string ImageMetadata::getFileName() const {
    return std::filesystem::path(filePath).filename().string();
}

std::string ImageMetadata::getFileTypeExtension() const {
    return std::filesystem::path(filePath).extension().string();
}

uint8_t ImageMetadata::getBitDepth() const {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "No se pudo abrir: " << filePath << "\n";
        return -1;
    }

    char header[32];
    file.read(header, sizeof(header));

    // PNG
    if (memcmp(header, "\x89PNG\r\n\x1a\n", 8) == 0) {
        file.seekg(24); // Byte 24: profundidad de bits en IHDR
        uint8_t bitDepth;
        file.read((char*)&bitDepth, 1);
        return bitDepth;
    }

    // BMP
    if (memcmp(header, "BM", 2) == 0) {
        file.seekg(28); // Byte 28: profundidad de bits
        char depthBytes[2];
        file.read(depthBytes, 2);
        return static_cast<uint16_t>((uint8_t)depthBytes[0] | ((uint8_t)depthBytes[1] << 8));
    }

    // GIF
    if (memcmp(header, "GIF87a", 6) == 0 || memcmp(header, "GIF89a", 6) == 0) {
        uint8_t packedField = header[10];
        int colorResolution = ((packedField & 0b01110000) >> 4) + 1;
        return colorResolution; // En bits por píxel
    }

    // JPG
    if ((uint8_t)header[0] == 0xFF && (uint8_t)header[1] == 0xD8) {
        file.seekg(2);
        while (!file.eof()) {
            uint8_t marker[2];
            file.read((char*)marker, 2);
            if (marker[0] != 0xFF) break;

            if (marker[1] >= 0xC0 && marker[1] <= 0xC3) { // SOF0-SOF3
                char data[6];
                file.read(data, 6);
                uint8_t bitDepth = (uint8_t)data[0]; // Precisión de bits por componente
                return bitDepth * 3; // Generalmente * 3 canales (R, G, B)
            } else {
                char lenBytes[2];
                file.read(lenBytes, 2);
                uint16_t len = ((uint8_t)lenBytes[0] << 8) | (uint8_t)lenBytes[1];
                file.seekg(len - 2, std::ios::cur);
            }
        }
        return -2; // No se pudo determinar
    }

    // WEBP (requiere análisis del chunk VP8X o VP8L para saber si es 8 o 24 bits)
    if (memcmp(header, "RIFF", 4) == 0 && memcmp(header + 8, "WEBP", 4) == 0) {
        // Para simplificar, asumimos 24 bits (no hay header directo)
        return 24; // Se puede mejorar si deseas leer chunk VP8X
    }

    return -3; // Formato no reconocido
    // return imageData.bitDepth;
}

std::string ImageMetadata::getColorType() const {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "No se puede abrir el archivo\n";
        return "Error al abrir archivo";
    }

    char header[32] = {};
    file.read(header, sizeof(header));
    file.close();

    // PNG
    if (memcmp(header, "\x89PNG\r\n\x1a\n", 8) == 0) {
        uint8_t colorType = header[25];
        switch (colorType) {
            case 0: return "Escala de grises";
            case 2: return "Truecolor (RGB)";
            case 3: return "Indexado";
            case 4: return "Escala de grises con alfa";
            case 6: return "Truecolor con alfa (RGBA)";
            default: return "Desconocido (PNG)";
        }
    }
    // JPG
    else if (memcmp(header, "\xFF\xD8\xFF", 3) == 0) {
        return "YCbCr (típicamente convertido a RGB)";
    }
    // GIF
    else if (memcmp(header, "GIF87a", 6) == 0 || memcmp(header, "GIF89a", 6) == 0) {
        return "Indexado";
    }
    // BMP
    else if (memcmp(header, "BM", 2) == 0) {
        uint16_t bpp = (uint8_t)header[28] | ((uint8_t)header[29] << 8);
        return "RGB (" + std::to_string(bpp) + "-bit)";
    }
    // WEBP
    else if (memcmp(header, "RIFF", 4) == 0 && memcmp(header + 8, "WEBP", 4) == 0) {
        return "RGB/RGBA (Compresión Lossy/Lossless)";
    }

    return "Desconocido";
}