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
#include "../metadata/VideoMetadata.h"

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
    unique_ptr<VideoMetadata> videoMetadata;

    string getFileType(const filesystem::path &filePath);

    string getFile(const filesystem::path &filePath);

    string processFile(const string &filePath);

    string deleteMetadata(const string &filePath);

public:
    FileEngine();

    ~FileEngine();

    future<string> processFileAsync(const string &filePath);

    future<string> deleteMetadaAsync(const string &filePath);
};

#endif