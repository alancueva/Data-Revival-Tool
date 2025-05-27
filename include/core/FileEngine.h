#ifndef FILE_ENGINE_H
#define FILE_ENGINE_H

#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <memory>
#include <future>
#include "../metadata/ImageMetadata.h"
#include "../metadata/DocumentMetadata.h"
#include "../metadata/AudioMetadata.h"

using namespace std;

/**
 * * @brief Clase que representa el motor de procesamiento de archivos.
 */
class FileEngine
{
private:
    /**
     * * @brief Punteros únicos para almacenar metadatos de diferentes tipos de archivos.
     * * @note Utiliza punteros únicos para gestionar la memoria de manera eficiente.
     */
    unique_ptr<ImageMetadata> imageMetadata;
    unique_ptr<DocumentMetadata> documentMetadata;
    unique_ptr<AudioMetadata> audioMetadata;

    string filePath;
    string getFileType(const filesystem::path &filePath);
    string processFile(const string &filePath);

public:
    FileEngine();
    ~FileEngine();

    future<string> processFileAsync(const string &filePath);
};

#endif