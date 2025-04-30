#include "../../include/metadata/FileMetadata.h"
#include <fstream>
#include <stdexcept>

FileMetadata::FileMetadata(const std::string& path) 
    : filePath(path), isValid(false), fileSize(0) {
    
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("No se puede acceder al archivo: " + path);
    }
    
    // Obtener el tamaño del archivo
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
}

bool FileMetadata::isFileValid() const {
    return isValid;
}

std::string FileMetadata::getFilePath() const {
    return filePath;
}

uint64_t FileMetadata::getFileSize() const {
    return fileSize;
}

std::string FileMetadata::getBasicInfo() const {
    std::string info = "Ruta: " + filePath + "\n";
    info += "Tamaño: " + std::to_string(fileSize) + " bytes\n";
    info += "Tipo: " + getFileType() + "\n";
    return info;
}

/**
 * metodo de Simulacion **Corregir**
 */
bool FileMetadata::analyze() {
    // Implementación provisional
    return true;
}

std::string FileMetadata::getFileType() const {
    // Implementación provisional
    return "Desconocido";
}

std::string FileMetadata::extractMetadata() {
    // Implementación provisional
    return "Metadata extraída";
}

std::string FileMetadata::recoverOverwrittenMetadata() {
    // Implementación provisional
    return "Metadata recuperada";
}