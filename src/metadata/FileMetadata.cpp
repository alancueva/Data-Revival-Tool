#include "../../include/metadata/FileMetadata.h"
#include <fstream>
#include <stdexcept>

FileMetadata::FileMetadata(const std::string& path) 
    : filePath(path), isValid(false), fileSize(0) {
    
    // Verificar que el archivo existe y es accesible
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