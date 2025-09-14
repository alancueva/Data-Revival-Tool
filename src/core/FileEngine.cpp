#include "../../include/core/FileEngine.h"
#include "../../include/metadata/ImageMetadata.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <filesystem>
#include <future>
#include <algorithm>
#include <cctype>

using namespace std;

FileEngine::FileEngine()
{
    /**
     * @brief Inicialización de los punteros únicos para almacenar metadatos de diferentes
     * tipos de archivos.
     * @note Utiliza punteros únicos para gestionar la memoria de manera eficiente.
     */
    imageMetadata = make_unique<ImageMetadata>();
    documentMetadata = make_unique<DocumentMetadata>();
    audioMetadata = make_unique<AudioMetadata>();
    videoMetadata = make_unique<VideoMetadata>();
}

FileEngine::~FileEngine() = default;

string FileEngine::getFileType(const filesystem::path &filepath_)
{
    string ext = filesystem::path(filepath_).extension().string();
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

/**
 *
 * @brief Método para analizar el tipo de archivo.
 * @param filePath Ruta del archivo a analizar.
 * @return string con el tipo de archivo.
 */
string FileEngine::getFile(const filesystem::path &filePath)
{
    string ext = filePath.extension().string();
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".txt" || ext == ".doc" || ext == ".docx" || ext == ".pdf" || ext == ".xls" ||
        ext == ".xlsx" || ext == ".ppt" || ext == ".pptx" || ext == ".csv" || ext == ".xml")
    {
        return "Documento";
    }
    else if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".gif" || ext == ".bmp")
    {
        return "Imagen";
    }
    else if (ext == ".mp3" || ext == ".wav" || ext == ".ogg" || ext == ".flac" || ext == ".aac")
    {
        return "Audio";
    }
    else if (ext == ".mp4" || ext == ".avi" || ext == ".mov" || ext == ".mkv" || ext == ".wmv" ||
             ext == ".flv" || ext == ".webm" || ext == ".m4v" || ext == ".3gp" || ext == ".3g2" ||
             ext == ".asf" || ext == ".f4v" || ext == ".m2v" || ext == ".mpg" || ext == ".mpeg" ||
             ext == ".mpe" || ext == ".vob" || ext == ".ogv" || ext == ".rm" || ext == ".rmvb" ||
             ext == ".ts" || ext == ".mts" || ext == ".m2ts" || ext == ".divx" || ext == ".xvid")
    {
        return "Video";
    }
    else if (ext == ".exe" || ext == ".bin" || ext == ".dll" || ext == ".so" || ext == ".app")
    {
        return "Ejecutable";
    }
    else if (ext == ".zip" || ext == ".rar" || ext == ".tar" || ext == ".gz")
    {
        return "Archivo Comprimido";
    }
    else if (ext == ".cpp" || ext == ".h" || ext == ".py" || ext == ".js" || ext == ".java" || ext == ".c" || ext == ".cs" || ext == ".go" || ext == ".rb" || ext == ".php" || ext == ".swift" || ext == ".xml" || ext == ".json" || ext == ".yaml" || ext == ".sql" || ext == ".pl" || ext == ".sh" || ext == ".bash")
    {
        return "Código Fuente";
    }
    else if (ext == ".html" || ext == ".css" || ext == ".js" || ext == ".php" || ext == ".asp" || ext == ".jsp")
    {
        return "Página Web";
    }
    else if (ext == ".iso" || ext == ".img" || ext == ".dmg")
    {
        return "Imagen de Disco";
    }
    else if (ext == ".log")
    {
        return "Archivo de Registro";
    }
    else if (ext == ".json")
    {
        return "Archivo JSON";
    }
    else
    {
        return "Desconocido";
    }
}

/**
 *------------------ METODO DE PROCESAMIENTO DE ARCHIVOS------------------
 * @brief Método para procesar el archivo.
 * @param filePath Ruta del archivo a procesar.
 * @return string con el resultado del procesamiento.
 * @details Este método analiza el tipo de archivo y extrae los metadatos correspondientes.
 * * - ifstream: Clase de C++ que permite leer archivos de forma eficiente.
 * * - stringstream: Clase de C++ que permite construir cadenas de texto de forma eficiente.
 * * - filesystem::path: Clase de C++ que representa una ruta de archivo en el sistema de archivos.
 */
string FileEngine::processFile(const string &filePath)
{
    filesystem::path path(filePath);
    string fileType = getFile(path);
    ifstream file(filePath, ios::binary);
    stringstream lstdata;
    if (file.is_open())
    {
        if (fileType == "Imagen")
        {
            lstdata << imageMetadata->extractMetadataImg(filePath);
        }
        else if (fileType == "Documento")
        {
            lstdata << documentMetadata->extractMetadata(filePath);
        }
        else if (fileType == "Audio")
        {
            lstdata << audioMetadata->getAllMetadata(filePath);
        }
        else if (fileType == "Video")
        {
            lstdata << videoMetadata->extractAllMetadata(filePath);
        }
        else if (fileType == "Ejecutable")
        {
            lstdata << "Procesamiento de ejecutables no implementado." << endl;
        }
        else if (fileType == "Archivo Comprimido")
        {
            lstdata << "Procesamiento de archivos comprimidos no implementado." << endl;
        }
        else if (fileType == "Código Fuente")
        {
            lstdata << "Procesamiento de código fuente no implementado." << endl;
        }
        else if (fileType == "Página Web")
        {
            lstdata << "Procesamiento de páginas web no implementado." << endl;
        }
        else if (fileType == "Imagen de Disco")
        {
            lstdata << "Procesamiento de imágenes de disco no implementado." << endl;
        }
        else if (fileType == "Archivo de Registro")
        {
            lstdata << "Procesamiento de archivos de registro no implementado." << endl;
        }
        else if (fileType == "Archivo JSON")
        {
            lstdata << "Procesamiento de archivos JSON no implementado." << endl;
        }
        else if (fileType == "Desconocido")
        {
            lstdata << "Tipo de archivo desconocido: " << filePath << endl;
        }
        else
        {
            lstdata << "Tipo de archivo no soportado: " << fileType << endl;
        }
        file.close();
    }
    else
    {
        lstdata << "Error al abrir archivo: " << filePath << endl;
        lstdata << "Posibles causas:" << endl;
        lstdata << "- El archivo no existe" << endl;
        lstdata << "- Sin permisos de lectura" << endl;
        lstdata << "- Archivo en uso por otra aplicación" << endl;
        lstdata << "- Ruta con caracteres especiales" << endl;
    }
    return lstdata.str();
}

string FileEngine::deleteMetadata(const string &filePath)
{

    string fileType = getFileType(filePath);
    stringstream cleanMethod;
    string tempFile = filePath + ".clean";
     bool cleanSuccess = false;

    if (fileType == ".jpg" || fileType == ".jpeg")
    {
        cleanSuccess = imageMetadata->cleanJPEGMetadata(filePath, tempFile);
        if(cleanSuccess){
            cleanMethod << "eliminación de metadatos EXIF/JPEG" << endl;
        }

    }
    else if (fileType == ".png")
    {
        cleanSuccess = imageMetadata->cleanPNGMetadata(filePath, tempFile);
        if(cleanSuccess){
            cleanMethod << "eliminación de chunks de metadatos PNG" << endl;    
        }
    }

    else
    {
        // Para tipos no soportados
        ifstream input(filePath, ios::binary);
        ofstream output(tempFile, ios::binary);

        if (input.is_open() && output.is_open())
        {
            output << input.rdbuf();
            cleanMethod << "copia simple (tipo no soportado para limpieza específica)" << endl;
        }
    }
    return cleanMethod.str();
}

/**
 * @brief Método para procesar el archivo de forma asíncrona.
 * @param filePath Ruta del archivo a procesar.
 * @return future<string> con el resultado del procesamiento.
 *
 * @details
 * - future<string>: Representa un valor que estará disponible en el futuro. Se utiliza para operaciones asíncronas
 *   donde el resultado no está disponible inmediatamente sino que se calculará más tarde.
 *
 * - async: Una plantilla de función que ejecuta una función de forma asíncrona (potencialmente en un hilo separado),
 *   y devuelve un future que eventualmente contendrá el resultado de esa función.
 *
 * - async(launch::async, [this, filePath]()):
 *   - launch::async fuerza la operación a ejecutarse en un nuevo hilo
 *   - [this, filePath]() es una función lambda que captura:
 *     - puntero 'this' para acceder a los miembros de la clase
 *     - parámetro filePath para pasar a processFile
 */
future<string> FileEngine::processFileAsync(const string &filePath)
{
    return async(launch::async, [this, filePath]()
                 { return this->processFile(filePath); });
}

future<string> FileEngine::deleteMetadaAsync(const string &filePath)
{
    return async(launch::async, [this, filePath]()
                 { return this->deleteMetadata(filePath); });
}