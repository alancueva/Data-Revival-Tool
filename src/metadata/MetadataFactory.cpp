#include "../../include/metadata/MetadataFactory.h"
#include "../../include/core/MetadataRecoveryEngine.h"
#include "../../include/metadata/ImageMetadata.h"
#include "../../include/metadata/DocumentMetadata.h"
#include <string>
#include <memory>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

MetadataFactory::MetadataFactory() {}

std::unique_ptr<FileMetadata> MetadataFactory::createAnalyzer(const std::string &filePath)
{
    std::string extension = filePath.substr(filePath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == "jpg" || extension == "jpeg" || extension == "png" || extension == "tiff" || extension == "bmp" || extension == "gif")
    {
        return std::make_unique<ImageMetadata>(filePath);
    }
    else if (extension == "pdf" || extension == "doc" || extension == "docx" || extension == "xls" || extension == "xlsx" || extension == "ppt" || extension == "pptx")
    {
        return std::make_unique<DocumentMetadata>(filePath);
    }
    else
    {
        // Intentar determinar el tipo de archivo basado en su contenido
        std::ifstream file(filePath, std::ios::binary);
        if (!file)
            return nullptr;

        // Leer los primeros bytes
        std::vector<uint8_t> header(12);
        file.read(reinterpret_cast<char *>(header.data()), 12);

        // Verificar cabeceras comunes
        if ((header[0] == 0xFF && header[1] == 0xD8) ||                                           // JPEG
            (header[0] == 0x89 && header[1] == 0x50 && header[2] == 0x4E && header[3] == 0x47) || // PNG
            (header[0] == 0x49 && header[1] == 0x49) || (header[0] == 0x4D && header[1] == 0x4D))
        { // TIFF
            return std::make_unique<ImageMetadata>(filePath);
        }
        else if ((header[0] == 0xD0 && header[1] == 0xCF && header[2] == 0x11 && header[3] == 0xE0) || // MS Office antiguo
                 (header[0] == 0x50 && header[1] == 0x4B && header[2] == 0x03 && header[3] == 0x04) || // Office XML o ZIP
                 (header[0] == 0x25 && header[1] == 0x50 && header[2] == 0x44 && header[3] == 0x46))
        { // PDF
            return std::make_unique<DocumentMetadata>(filePath);
        }
    }

    // Tipo de archivo no soportado
    return nullptr;
}