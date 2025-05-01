#include "../../include/metadata/FileMetadata.h"
#include <fstream>
#include <stdexcept>
using namespace std;

FileMetadata::FileMetadata(const string& path) 
    : filePath(path), isValid(false), fileSize(0) {
    
    ifstream file(path, ios::binary);
    if (!file) {
        throw runtime_error("No se puede acceder al archivo: " + path);
    }
    
    // Obtener el tamaño del archivo
    file.seekg(0, ios::end);
    fileSize = file.tellg();
    file.seekg(0, ios::beg);
}

bool FileMetadata::isFileValid() const {
    return isValid;
}

string FileMetadata::getFilePath() const {
    return filePath;
}

uint64_t FileMetadata::getFileSize() const {
    return fileSize;
}

string FileMetadata::getBasicInfo() const {
    string info = "Ruta: " + filePath + "\n";
    info += "Tamaño: " + to_string(fileSize) + " bytes\n";
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

string FileMetadata::getFileType() const {
    // Implementación provisional
    return "Desconocido";
}

/**
 * metodo de Simulacion **Corregir**
 */
string FileMetadata::extractMetadata() {

    // if (!isValid) {
    //     return "Archivo no válido o no se puede extraer metadata";
    // }
    // Implementación provisional
    string metadata = "== METADATOS EXTRAÍDOS ==\n\n";
    
    metadata += "Ejemplo de metadata: [simulación]\n";

    return metadata;
}

string FileMetadata::recoverOverwrittenMetadata() {
    // Implementación provisional
    return "Metadata recuperada";
}