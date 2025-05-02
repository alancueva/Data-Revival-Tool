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

FileMetadata::FileMetadata(const string &path)
    : filePath(path), isValid(false), fileSize(0)
{
    //, magic(nullptr)
    cout << "Creando objeto FileMetadata para el archivo: " << path << endl;
    ifstream file(path, ios::binary);
    if (!file)
    {
        throw runtime_error("No se puede acceder al archivo: " + path);
    }

    // Obtener el tamaño del archivo
    file.seekg(0, ios::end);
    fileSize = file.tellg();
    file.seekg(0, ios::beg);
}

bool FileMetadata::isFileValid() const
{
    return isValid;
}

string FileMetadata::getFilePath() const
{
    return filePath;
}

uint64_t FileMetadata::getFileSize() const
{
    return fileSize;
}

string FileMetadata::getBasicInfo()
{
    string info = "Ruta: " + filePath + "\n";
    info += "Tamaño: " + to_string(fileSize) + " bytes\n";
    info += "Tipo: " + getFileType(filePath) + "\n";
    return info;
}

/**
 * metodo de Simulacion **Corregir**
 */
bool FileMetadata::analyze()
{
    // Implementación provisional
    return true;
}

/**
 * @brief Método para analizar el archivo y extraer metadatos.
 * @return string con los metadatos extraídos.
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
    else if (ext == ".cpp" || ext == ".h" || ext == ".py" || ext == ".js" || ext == ".java" || ext == ".c" || ext == ".cs" || ext == ".go" || ext == ".rb" || ext == ".php" || ext == ".swift" || ext == ".html" || ext == ".css" || ext == ".xml" || ext == ".json" || ext == ".yaml" || ext == ".sql" || ext == ".pl" || ext == ".sh" || ext == ".bash")
    {
        return "Código Fuente";
    }
    else
    {
        return "Desconocido";
    }
}

/**
 * * @brief Extrae los metadatos del archivo.
 * @return string con los metadatos extraídos.
 */
string FileMetadata::extractMetadata()
{
    string metadata = "== METADATOS EXTRAÍDOS ==\n\n";

    metadata += "Nombre del archivo: " + getFileName(filePath) + "\n";
    metadata += "Tamaño del archivo: " + to_string(fileSize) + " bytes\n";
    metadata += "Extensión del archivo: " + getFileExtension(filePath) + "\n";

    time_t modTime = getFileLastModified(filePath);
    ostringstream oss;
    oss << put_time(localtime(&modTime), "%Y-%m-%d %H:%M:%S");
    string creationDate = oss.str();

    metadata += "Última modificación: " + creationDate + "\n";
    metadata += "Tipo de archivo: " + getFileType(filePath) + "\n";
    metadata += "Ruta del archivo: " + filePath + "\n";
    auto perms = filesystem::status(filePath).permissions();
    metadata += "Permisos: " + to_string((int)perms) + "\n";
    metadata += "Tamaño de permisos: " + to_string((int)perms) + "\n";
    metadata += "Fecha de creación: " + getFileCreationDate(filePath) + "\n";
    return metadata;
}

string FileMetadata::recoverOverwrittenMetadata()
{
    // Implementación provisional
    return "Metadata recuperada";
}

/**
 * * @brief Obtiene el nombre del archivo a partir de su ruta.
 * @param filePath Ruta del archivo.
 */

string FileMetadata::getFileName(const filesystem::path &filePath)
{
    return filePath.filename().string();
}

string FileMetadata::getFileExtension(const filesystem::path &filePath)
{
    return filePath.extension().string();
}

time_t FileMetadata::getFileLastModified(const filesystem::path &filePath)
{
    filesystem::file_time_type modTime = filesystem::last_write_time(filePath);
    auto sctp = chrono::system_clock::now() +
                chrono::duration_cast<chrono::system_clock::duration>(
                    modTime - decltype(modTime)::clock::now());

    return chrono::system_clock::to_time_t(sctp);
}


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