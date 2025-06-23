
#include "../../include/core/MetadataRecoveryEngine.h"
#include "../../include/core/FileEngine.h"
#include <string>
#include <iostream>
#include <memory>
#include <filesystem>
#include <future>
#include <algorithm>
#include <cctype>

using namespace std;

MetadataRecoveryEngine::MetadataRecoveryEngine()
{
    fileEngine = make_unique<FileEngine>();
    filePath = "";
}

MetadataRecoveryEngine::~MetadataRecoveryEngine() = default;

/**
 * @brief Ruta de la imagen.
 * @param filePath Ruta de la imagen.
 * @return string con la ruta de la imagen.
 */
string MetadataRecoveryEngine::routeImage(const filesystem::path &filePath)
{
    this->filePath = filePath.string();
    return this->filePath;
}

/**
 * @brief Recupera los metadatos de un archivo dado.
 * @param filePath Ruta del archivo.
 * @return string con los metadatos recuperados.
 * @note Este método utiliza la clase FileEngine para extraer los metadatos del archivo.
 */
string MetadataRecoveryEngine::recoverMetadata(const filesystem::path &filePath)
{
    string routestr = routeImage(filePath);
    try
    {
        auto future = fileEngine->processFileAsync(routestr);
        // Esto puede lanzar una excepción si ocurre algo en processFile
        // Por ejemplo, si el archivo no se puede abrir.
        // Si esto ocurre, el catch manejará la excepción y devolverá un mensaje de error.
        string resultado = future.get();
        return resultado.c_str();
    }
    catch (const exception &e)
    {
        cerr << "Error al recuperar metadatos: " << e.what() << endl;
        return "Error en recuperación de metadatos";
    }
}

string MetadataRecoveryEngine::deleteMetadata(const string &filepath)
{
    string routestr = routeImage(filePath);
    try
    {
        auto future = fileEngine->deleteMetadaAsync(routestr);
        // Esto puede lanzar una excepción si ocurre algo en processFile
        // Por ejemplo, si el archivo no se puede abrir.
        // Si esto ocurre, el catch manejará la excepción y devolverá un mensaje de error.
        string resultado = future.get();
        return resultado.c_str();
    }
    catch (const exception &e)
    {
        cerr << "Error al Eliminar metadatos: " << e.what() << endl;
        return "Error en Eliminar de metadatos";
    }
}
