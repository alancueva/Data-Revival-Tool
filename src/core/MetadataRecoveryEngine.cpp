
#include "../../include/metadata/FileMetadata.h"
#include "../../include/metadata/MetadataFactory.h"
#include "../../include/core/MetadataRecoveryEngine.h"
#include <string>
#include <memory>
using namespace std;

MetadataRecoveryEngine::MetadataRecoveryEngine():
    fileMetadata(new FileMetadata())
{

}

MetadataRecoveryEngine::~MetadataRecoveryEngine()
{
    delete fileMetadata;
}

/**
 * @brief Recupera los metadatos de un archivo dado.
 * @param filePath Ruta del archivo.
 * @return string con los metadatos recuperados.
 * @note Este método utiliza la clase FileMetadata para extraer los metadatos del archivo.
 */
string MetadataRecoveryEngine::recoverMetadata(const filesystem::path &filePath){

    // if (!filePath.empty())
    //     return "Ningún archivo cargado";
    // if (!filesystem::exists(filePath))
    //     return "El archivo no existe";
    string fileType = fileMetadata->extractMetadata(filePath);
    return fileType;
}

