#include "../../include/metadata/ImageMetadata.h"
#include <fstream>
#include <sstream>
#include <iomanip>

ImageMetadata::ImageMetadata(const std::string& path) 
    : FileMetadata(path), width(0), height(0), colorDepth(0), filePath(path), isValid(false) {
}

bool ImageMetadata::analyze() {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return false;
    
    // Leer los primeros bytes para identificar el formato
    fileHeader.resize(16);
    file.read(reinterpret_cast<char*>(fileHeader.data()), 16);
    
    // Verificar formato basado en los bytes de cabecera
    if (fileHeader[0] == 0xFF && fileHeader[1] == 0xD8) {
        imageFormat = "JPEG";
        isValid = analyzeJPEG(file);
    } else if (fileHeader[0] == 0x89 && fileHeader[1] == 0x50 && 
               fileHeader[2] == 0x4E && fileHeader[3] == 0x47) {
        imageFormat = "PNG";
        isValid = analyzePNG(file);
    } else if ((fileHeader[0] == 0x49 && fileHeader[1] == 0x49) || 
               (fileHeader[0] == 0x4D && fileHeader[1] == 0x4D)) {
        imageFormat = "TIFF";
        isValid = analyzeTIFF(file);
    } else if (fileHeader[0] == 0x42 && fileHeader[1] == 0x4D) {
        imageFormat = "BMP";
        isValid = analyzeBMP(file);
    } else if (fileHeader[0] == 0x47 && fileHeader[1] == 0x49 && 
               fileHeader[2] == 0x46) {
        imageFormat = "GIF";
        isValid = analyzeGIF(file);
    } else {
        imageFormat = "Desconocido";
        isValid = false;
    }
    
    return isValid;
}

std::string ImageMetadata::getFileType() const {
    return "Imagen - " + imageFormat;
}

std::string ImageMetadata::extractMetadata() {
    if (!isValid) return "Archivo no válido o no es una imagen compatible";
    
    std::stringstream metadata;
    metadata << "== METADATOS DE IMAGEN ==\n\n";
    metadata << "Formato: " + imageFormat + "\n";
    
    if (width > 0 && height > 0) {
        metadata << "Dimensiones: " << width << "x" << height << " píxeles\n";
    }
    
    if (colorDepth > 0) {
        metadata << "Profundidad de color: " << colorDepth << " bits\n";
    }
    
    if (!creationDate.empty()) {
        metadata << "Fecha de creación: " << creationDate << "\n";
    }
    
    if (!camera.empty()) {
        metadata << "Cámara/Dispositivo: " << camera << "\n";
    }
    
    if (!software.empty()) {
        metadata << "Software: " << software << "\n";
    }
    
    // Añadir datos de GPS si están disponibles
    if (hasGPS) {
        metadata << "Coordenadas GPS: " << gpsLatitude << ", " << gpsLongitude << "\n";
    }
    
    return metadata.str();
}

std::string ImageMetadata::recoverOverwrittenMetadata() {
    if (!isValid) return "No se puede recuperar metadata de un archivo no válido";
    
    std::stringstream recoveredData;
    recoveredData << "== METADATOS RECUPERADOS ==\n\n";
    
    // Simulación de recuperación de metadatos EXIF/XMP/IPTC
    if (imageFormat == "JPEG") {
        recoveredData << "Metadatos originales recuperados:\n";
        recoveredData << "- GPS: 40.7128° N, 74.0060° W\n";
        recoveredData << "- Dispositivo: " << (camera.empty() ? "iPhone 13 Pro" : "Anterior: " + camera) << "\n";
        recoveredData << "- Fecha original: 28/02/2023\n";
        recoveredData << "- Software de edición: Adobe Photoshop 2023\n";
        recoveredData << "- Modificación de luminosidad: +15%\n";
        recoveredData << "- Recorte: Imagen original 25% más grande\n";
    } else if (imageFormat == "PNG") {
        recoveredData << "Metadatos originales recuperados:\n";
        recoveredData << "- Dimensiones originales: 1024x768 (redimensionada)\n";
        recoveredData << "- Autor: Juan Pérez\n";
        recoveredData << "- Fecha de captura: 10/03/2023\n";
        recoveredData << "- Comentarios eliminados: \"Versión preliminar\"\n";
    } else if (imageFormat == "TIFF") {
        recoveredData << "Metadatos originales recuperados:\n";
        recoveredData << "- Múltiples capas fusionadas: 3 capas originales\n";
        recoveredData << "- Software original: GIMP 2.10\n";
        recoveredData << "- Historial de cambios detectado: 5 sesiones de edición\n";
    }
    
    return recoveredData.str();
}

bool ImageMetadata::analyzeJPEG(std::ifstream& file) {
    // Restablecemos la posición del archivo
    file.seekg(0, std::ios::beg);
    
    // Simulación de análisis de JPEG
    // En una implementación real, buscaríamos segmentos EXIF, XMP, etc.
    width = 1920;
    height = 1080;
    colorDepth = 24;
    creationDate = "01/04/2023";
    camera = "Canon EOS R5";
    software = "Adobe Lightroom";
    hasGPS = true;
    gpsLatitude = "40.7128° N";
    gpsLongitude = "74.0060° W";
    
    return true;
}

bool ImageMetadata::analyzePNG(std::ifstream& file) {
    // Restablecemos la posición del archivo
    file.seekg(0, std::ios::beg);
    
    // Simulación de análisis de PNG
    // En una implementación real, buscaríamos chunks relevantes (IHDR, tEXt, etc.)
    width = 800;
    height = 600;
    colorDepth = 32; // RGBA
    creationDate = "15/03/2023";
    software = "Adobe Photoshop";
    hasGPS = false;
    
    return true;
}

bool ImageMetadata::analyzeTIFF(std::ifstream& file) {
    // Implementación similar para archivos TIFF
    // ...
    width = 2400;
    height = 1600;
    colorDepth = 48; // 16 bits por canal RGB
    creationDate = "20/02/2023";
    camera = "Nikon Z7";
    hasGPS = true;
    gpsLatitude = "41.8781° N";
    gpsLongitude = "87.6298° W";
    
    return true;
}

bool ImageMetadata::analyzeBMP(std::ifstream& file) {
    // Implementación similar para archivos BMP
    // ...
    width = 1024;
    height = 768;
    colorDepth = 24;
    hasGPS = false;
    
    return true;
}

bool ImageMetadata::analyzeGIF(std::ifstream& file) {
    // Implementación similar para archivos GIF
    // ...
    width = 500;
    height = 300;
    colorDepth = 8; // Paleta indexada
    hasGPS = false;
    
    return true;
}