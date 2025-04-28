#pragma once

#include <memory>
#include <string>
#include "FileMetadata.h"

/**
 * Factory para crear el tipo adecuado de analizador de metadatos
 */
class MetadataFactory {
public:
    static std::unique_ptr<FileMetadata> createAnalyzer(const std::string& filePath);
};
