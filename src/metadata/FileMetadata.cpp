#include "../../include/metadata/FileMetadata.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

FileMetadata::FileMetadata() 
{

}

FileMetadata::~FileMetadata()
{

}


/**
 * @brief Método para analizar el archivo y extraer metadatos.
 * @return string con los metadatos extraídos.
 * @note
    Este archivo implementa la clase y métodos relacionados con la obtención de metadatos de archivos.
    En particular, el método getFileType se utiliza para identificar el tipo de archivo (por ejemplo, texto, imagen, binario, etc.).
    Esta identificación permite que, en otras partes del proyecto, se puedan delegar las operaciones específicas de cada tipo de archivo
    a implementaciones especializadas y separadas. Así, cada tipo de archivo será gestionado por su propia clase o módulo,
    promoviendo la modularidad y facilitando el mantenimiento y la extensión del sistema.
 */
string FileMetadata::getFileType(const std::filesystem::path &filePath)
{
    string ext = filePath.extension().string();
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower); // a minúsculas

    if (ext == ".txt" || ext == ".doc" || ext == ".docx" || ext == ".pdf" || ext == ".xls" || ext == ".xlsx" || ext == ".ppt" || ext == ".pptx" || ext == ".csv" || ext == ".xml")
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
    else if (ext == ".mp4" || ext == ".avi" || ext == ".mov" || ext == ".mkv" || ext == ".wmv" || ext == ".flv" || ext == ".webm")
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
    else if (ext == ".cpp" || ext == ".h" || ext == ".py" || ext == ".js" || ext == ".java" 
        || ext == ".c" || ext == ".cs" || ext == ".go" || ext == ".rb" || ext == ".php" 
        || ext == ".swift" || ext == ".xml" 
        || ext == ".json" || ext == ".yaml" || ext == ".sql" || ext == ".pl" || ext == ".sh" 
        || ext == ".bash")
    {
        return "Código Fuente";
    }
    else if (ext == ".html" || ext == ".css" || ext == ".js" 
        || ext == ".php" || ext == ".asp" || ext == ".jsp")
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
 * --------------------------EXTRACION DE METADATA BASICO--------------------------
 * @brief Extrae los metadatos del archivo.
 * @param filePath Ruta del archivo.
 * @return string con los metadatos extraídos.
 */
string FileMetadata::extractMetadata(const std::filesystem::path &filePath)
{
    string metadata = "== METADATOS EXTRAÍDOS ==\n\n";
    metadata += "Nombre del archivo: " + getFileName(filePath) + "\n";
    metadata += "Tamaño del archivo: " + getFileSize(filePath) + "\n";
    metadata += "Extensión del archivo: " + getFileExtension(filePath) + "\n";
    metadata += "Última modificación: " + getFileLastModifiedstr(filePath) + "\n";
    metadata += "Tipo de archivo: " + getFileType(filePath) + "\n";
    // metadata += "Ruta del archivo: " + filePath + "\n";
    metadata += "Permisos: " + getFilePermissions(filePath) + "\n";
    // metadata += "Tamaño de permisos: " + getFilePermissions(filePath) + "\n";
    metadata += "Fecha de creación: " + getFileCreationDate(filePath) + "\n";
    return metadata;
}


string FileMetadata::getFileSize(const filesystem::path &filePath)
{
    uintmax_t size = filesystem::file_size(filePath);
    string sizeStr;
    const char* units[] = {"bytes", "KB", "MB", "GB", "TB"};
    int unitIdx = 0;
    double displaySize = static_cast<double>(size);
    while (displaySize >= 1024 && unitIdx < 4) {
        displaySize /= 1024;
        ++unitIdx;
    }
    ostringstream sizeStream;
    sizeStream.precision(2);
    sizeStream << fixed << displaySize << " " << units[unitIdx];
    sizeStr = sizeStream.str();
    return sizeStr;
}


/**
 * * @brief Obtiene el nombre del archivo a partir de su ruta.
 * @param filePath Ruta del archivo.
 */

string FileMetadata::getFileName(const filesystem::path &filePath)
{
    return filePath.filename().string();
}

/**
 * @brief Obtiene la extensión del archivo a partir de su ruta.
 * @param filePath Ruta del archivo.
 */
string FileMetadata::getFileExtension(const filesystem::path &filePath)
{
    return filePath.extension().string();
}

/**
 * @brief Obtiene la fecha de última modificación del archivo.
 * @param filePath Ruta del archivo.
 */
time_t FileMetadata::getFileLastModified(const filesystem::path &filePath)
{
    filesystem::file_time_type modTime = filesystem::last_write_time(filePath);
    auto sctp = chrono::system_clock::now() +
                chrono::duration_cast<chrono::system_clock::duration>(
                    modTime - decltype(modTime)::clock::now());

    return chrono::system_clock::to_time_t(sctp);
}

/**
 * @brief Obtiene la fecha de última modificación del archivo como string.
 * @param filePath Ruta del archivo.
 */
string FileMetadata::getFileLastModifiedstr(const filesystem::path &filePath)
{
    time_t modTime = getFileLastModified(filePath);
    ostringstream oss;
    oss << put_time(localtime(&modTime), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

/**
 * @brief Obtiene los permisos del archivo.
 * @param filePath Ruta del archivo.
 * @return string con los permisos del archivo.
 */
string FileMetadata::getFilePermissions(const filesystem::path &filePath)
{
    error_code ec;
    auto perms = filesystem::status(filePath, ec).permissions();
    if (ec)
        return "Error al obtener permisos";

    ostringstream oss;
    oss << ((perms & filesystem::perms::owner_read) != filesystem::perms::none ? "r" : "-")
        << ((perms & filesystem::perms::owner_write) != filesystem::perms::none ? "w" : "-")
        << ((perms & filesystem::perms::owner_exec) != filesystem::perms::none ? "x" : "-")
        << ((perms & filesystem::perms::group_read) != filesystem::perms::none ? "r" : "-")
        << ((perms & filesystem::perms::group_write) != filesystem::perms::none ? "w" : "-")
        << ((perms & filesystem::perms::group_exec) != filesystem::perms::none ? "x" : "-")
        << ((perms & filesystem::perms::others_read) != filesystem::perms::none ? "r" : "-")
        << ((perms & filesystem::perms::others_write) != filesystem::perms::none ? "w" : "-")
        << ((perms & filesystem::perms::others_exec) != filesystem::perms::none ? "x" : "-");

    return oss.str();
}

/**
 * @brief Obtiene la fecha de creación del archivo.
 * @param filePath Ruta del archivo.
 */
string FileMetadata::getFileCreationDate(const filesystem::path &filePath)
{
    struct stat attr;
    if (stat(filePath.string().c_str(), &attr) != 0)
    {
        return "Error al obtener la fecha";
    }

    #if defined(__APPLE__)
        time_t ctime = attr.st_ctimespec.tv_sec;
    #elif defined(__linux__)
        time_t ctime = attr.st_ctim.tv_sec;
    #else
        time_t ctime = attr.st_ctime;
    #endif

    tm *timeinfo = localtime(&ctime);
    ostringstream oss;
    oss << put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}