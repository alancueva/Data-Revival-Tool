#ifndef METADATA_FACTORY_H
#define METADATA_FACTORY_H

#include <memory>
#include <string>
#include "FileMetadata.h"

/**
 * @brief Fábrica para crear analizadores de metadatos adecuados al tipo de archivo.
 * 
 * Esta clase decide qué analizador concreto utilizar basado en la extensión o contenido del archivo.
 */
class MetadataFactory {
public:
    MetadataFactory();
    /**
     * @brief Crea un analizador de metadatos específico para el archivo dado.
     * @param filePath Ruta al archivo a analizar.
     * @return Puntero único a un objeto FileMetadata adecuado, o nullptr si no se reconoce el tipo.
     */
    static std::unique_ptr<FileMetadata> createAnalyzer(const std::string& filePath);


};
#endif