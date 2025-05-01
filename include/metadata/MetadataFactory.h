#ifndef METADATA_FACTORY_H
#define METADATA_FACTORY_H

#include <memory>
#include <string>
#include "FileMetadata.h"
using namespace std;

/**
 * @brief Fábrica para crear analizadores de metadatos adecuados al tipo de archivo.
 * 
 * Esta clase decide qué analizador concreto utilizar basado en la extensión o contenido del archivo.
 */
class MetadataFactory {
public:
    /**
     * @brief Constructor de la clase MetadataFactory.
     */
    MetadataFactory();
    /**
     * @brief Destructor de la clase MetadataFactory.
     */
    //~MetadataFactory();
    /**
     * @brief Crea un analizador de metadatos específico para el archivo dado.
     * @param filePath Ruta al archivo a analizar.
     * @return Puntero único a un objeto FileMetadata adecuado, o nullptr si no se reconoce el tipo.
     */
    static unique_ptr<FileMetadata> createAnalyzer(const string& filePath);


};
#endif