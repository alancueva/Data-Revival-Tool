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
using namespace std;

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

string ImageMetadata::routeImage(const filesystem::path &filePath) {
    this->filePath = filePath.string();
    return this->filePath;
}


/**
 * ---------------------- METODO QUE EXTRAERÁ DETALLES DE LA IMAGEN ------------------------------
 * @brief Método para analizar el archivo y extraer metadatos.
 * @return string con los metadatos extraídos.
 */
string ImageMetadata::extractMetadataImg(const filesystem::path &filePath) {
    routeImage(filePath);
    stringstream metadata;
    metadata << "== METADATOS DE IMAGEN ==\n\n"
             //          << "Metadata del archivo PNG:" << "\n\n";
             << "Nombre del archivo: " << getFileName() << "\n"
             << "Tamaño del archivo: " << getFormattedFileSize() << "\n"
             << "Tipo de archivo: " << getFileType() << "\n"
             << "Extensión: " << getFileTypeExtension() << "\n"
             // metadata << "MIME Type: " << extractor.getMIMEType() << "\n";
             // metadata << "Ancho de imagen: " << extractor.getImageWidth() << "\n";
             // metadata << "Alto de imagen: " << extractor.getImageHeight() << "\n";
             << "Profundidad de bits: " << static_cast<int>(getBitDepth()) << "\n"
             << "Tipo de color: " << getColorType() << "\n"
             // metadata << "Compresión: " << extractor.getCompression() << "\n";
             // metadata << "Filtro: " << extractor.getFilter() << "\n";
             // metadata << "Entrelazado: " << extractor.getInterlace() << "\n";
             // metadata << "Renderizado sRGB: " << extractor.getSRGBRendering() << "\n";
             // metadata << "Gamma: " << extractor.getGamma() << "\n";
             << "Dimensiones de la Imagen: " << getDimensionImage() << "\n"
             // metadata << "Píxeles por unidad X: " << extractor.getPixelsPerUnitX() << "\n";
             // metadata << "Píxeles por unidad Y: " << extractor.getPixelsPerUnitY() << "\n";
             << "Unidades de píxel: " << getPixelUnits() << "\n";
    //<< "Tamaño de imagen: " << getImageSize() << "\n";
    // metadata << "Megapíxeles: " << extractor.getMegapixels() << "\n";
    // metadata << "Categoría: " << extractor.getCategory() << "\n";
    // metadata << "Raw Header: " << extractor.getRawHeader() << "\n";
    // if (extract(filePath))
    // {
    //     metadata << getAllMetadata();
    // }
    // else
    // {
    //     metadata << "Error al leer metadatos";
    // }
    //getDimensionImage();
    return metadata.str();
}


string ImageMetadata::getPixelUnits() const {
    return imageData.pixelUnits;
}

double ImageMetadata::getMegapixels() const {
    return static_cast<double>(imageData.width * imageData.height) / 1000000.0;
}

string ImageMetadata::getFileType() {
    ifstream file(filePath, ios::binary);
    if (!file) {
        cerr << "No se pudo abrir el archivo: " << filePath << "\n";
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


string ImageMetadata::getDimensionImage() {
    ifstream file(filePath, ios::binary);
    if (!file) {
        cerr << "No se pudo abrir el archivo: " << filePath << "\n";
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

        std::cout << "[PNG] Dimensiones: " << width << "x" << height << "\n";
        return to_string(width) + "x" + to_string(height);
    }

    // ----------------------------
    // BMP: firma "BM"
    if (memcmp(header, "BM", 2) == 0) {
        uint32_t width = *(int32_t*)&header[18];
        uint32_t height = *(int32_t*)&header[22];

        std::cout << "[BMP] Dimensiones: " << width << "x" << height << "\n";
        return to_string(width) + "x" + to_string(height);
    }

    // ----------------------------
    // GIF: firma "GIF87a" o "GIF89a"
    if (memcmp(header, "GIF87a", 6) == 0 || memcmp(header, "GIF89a", 6) == 0) {
        uint16_t width = (uint8_t)header[6] | ((uint8_t)header[7] << 8);
        uint16_t height = (uint8_t)header[8] | ((uint8_t)header[9] << 8);

        cout << "[GIF] Dimensiones: " << width << "x" << height << "\n";
        return to_string(width) + "x" + to_string(height);
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
                cout << "[JPG] Dimensiones: " << width << "x" << height << "\n";
                return to_string(width) + "x" + to_string(height);
            } else {
                char lenBytes[2];
                file.read(lenBytes, 2);
                uint16_t blockLength = ((uint8_t)lenBytes[0] << 8) | (uint8_t)lenBytes[1];
                file.seekg(blockLength - 2, ios::cur);
            }
        }

        cout << "[JPG] No se pudo leer dimensiones.\n";
        return "";
    }

    cout << "Formato desconocido o no soportado.\n";
    return "";
}



// bool ImageMetadata::extract(const filesystem::path &filePath) {
//     ifstream file(filePath, ios::binary);
//     if (!file) {
//         std::cerr << "Error: No se pudo abrir el archivo." << std::endl;
//         return false;
//     }

//     // Leer los primeros bytes para identificar el formato
//     char header[12] = {};
//     file.read(header, 12);
//     if (!file) {
//         std::cerr << "Error: No se pudieron leer los primeros bytes del archivo." << std::endl;
//         return false;
//     }

//     if (std::memcmp(header, "\x89PNG\r\n\x1a\n", 8) == 0) {
//         std::cout << "Es una imagen PNG.\n";
//     } else if (std::memcmp(header, "\xFF\xD8\xFF", 3) == 0) {
//         std::cout << "Es una imagen JPG.\n";
//     } else if (std::memcmp(header, "GIF87a", 6) == 0 || std::memcmp(header, "GIF89a", 6) == 0) {
//         std::cout << "Es una imagen GIF.\n";
//     } else if (std::memcmp(header, "BM", 2) == 0) {
//         std::cout << "Es una imagen BMP.\n";
//     } else if ((std::memcmp(header, "II*\x00", 4) == 0) || (std::memcmp(header, "MM\x00*", 4) == 0)) {
//         std::cout << "Es una imagen TIFF.\n";
//     } else if (std::memcmp(header, "RIFF", 4) == 0 && std::memcmp(header + 8, "WEBP", 4) == 0) {
//         std::cout << "Es una imagen WEBP.\n";
//     } else {
//         std::cout << "Tipo de imagen desconocido.\n";
//     }


//     // Detectar el formato del archivo
//     if (header[0] == (char)0xFF && header[1] == (char)0xD8) {
//         std::cout << "Formato detectado: JPEG" << std::endl;
//         return true;
//     } else if (header[0] == (char)0x89 && header[1] == 'P' && header[2] == 'N' && header[3] == 'G') {
//         std::cout << "Formato detectado: PNG" << std::endl;
//         return true;
//     } else {
//         std::cerr << "Error: Formato de archivo no soportado." << std::endl;
//         return false;
//     }
// }


// Método para obtener el tamaño del archivo en bytes
int64_t ImageMetadata::getFileSize() const {
    ifstream fs(filePath, ios::binary | ios::ate);
    if (!fs) return -1;
    return fs.tellg();
}

// Método para formatear el tamaño del archivo como string (KB, MB, etc.)
string ImageMetadata::getFormattedFileSize() const {
    int64_t size = getFileSize();
    if (size < 0) return "Unknown";

    if (size < 1024) {
        return to_string(size) + " B";
    } else if (size < 1024 * 1024) {
        return to_string(size / 1024) + " kB";
    } else {
        return to_string(size / (1024 * 1024)) + " MB";
    }
}

string ImageMetadata::getFileName() const {
    size_t pos = filePath.find_last_of("/\\");
    return (pos != string::npos) ? filePath.substr(pos + 1) : filePath;
}

string ImageMetadata::getFileTypeExtension() const {
    size_t pos = filePath.find_last_of(".");
    return (pos != string::npos) ? filePath.substr(pos + 1) : "";
}

uint8_t ImageMetadata::getBitDepth() const {
    ifstream file(filePath, ios::binary);
    if (!file) {
        std::cerr << "No se pudo abrir: " << filePath << "\n";
        return -1;
    }

    char header[32];
    file.read(header, sizeof(header));

    // PNG
    if (std::memcmp(header, "\x89PNG\r\n\x1a\n", 8) == 0) {
        file.seekg(24); // Byte 24: profundidad de bits en IHDR
        uint8_t bitDepth;
        file.read((char*)&bitDepth, 1);
        return bitDepth;
    }

    // BMP
    if (std::memcmp(header, "BM", 2) == 0) {
        file.seekg(28); // Byte 28: profundidad de bits
        uint16_t bitDepth;
        file.read((char*)&bitDepth, 2);
        return bitDepth;
    }

    // GIF
    if (std::memcmp(header, "GIF87a", 6) == 0 || std::memcmp(header, "GIF89a", 6) == 0) {
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
    if (std::memcmp(header, "RIFF", 4) == 0 && std::memcmp(header + 8, "WEBP", 4) == 0) {
        // Para simplificar, asumimos 24 bits (no hay header directo)
        return 24; // Se puede mejorar si deseas leer chunk VP8X
    }

    return -3; // Formato no reconocido
    // return imageData.bitDepth;
}

string ImageMetadata::getColorType() const {
    ifstream file(filePath, ios::binary);
    if (!file) {
        cerr << "No se puede abrir el archivo\n";
        return "\n";
    }
    stringstream metadata;

    file.seekg(10); // Offset a los datos de píxeles
    uint32_t pixelOffset;
    file.read((char*)&pixelOffset, 4);

    file.seekg(18); // Dimensiones
    int32_t width, height;
    file.read((char*)&width, 4);
    file.read((char*)&height, 4);

    file.seekg(28); // Bits por píxel
    uint16_t bpp;
    file.read((char*)&bpp, 2);

    if (bpp != 24) {
        cerr << "Solo soportado BMP de 24 bits (sin compresión)\n";
        metadata << "No soportado \n";
        return metadata.str();
    }

    // Ir a donde empiezan los píxeles
    file.seekg(pixelOffset);

    int rowPadding = (4 - (width * 3) % 4) % 4; // Cada fila debe alinear a 4 bytes

    cout << "Primeros colores en hexadecimal (formato #RRGGBB):\n";

    for (int y = 0; y < min(height, 5); ++y) { // solo primeras 5 filas
        for (int x = 0; x < min(width, 5); ++x) { // primeras 5 columnas
            uint8_t bgr[3];
            file.read((char*)bgr, 3);
            metadata << "#" << hex << setw(2) << setfill('0')
                     << (int)bgr[2]  // Red
                     << setw(2) << (int)bgr[1]  // Green
                     << setw(2) << (int)bgr[0]  // Blue
                     << " ";
        }
        file.ignore(rowPadding); // Saltar padding
        metadata << "\n"; // Nueva fila en el tex
    }
    return metadata.str();
}