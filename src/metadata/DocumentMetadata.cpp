#include "../../include/metadata/DocumentMetadata.h"
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

DocumentMetadata::DocumentMetadata()
{
    m_filePath = "";
    m_metadata["signature"] = "";
    m_metadata["mime_type"] = "";
    m_metadata["file_name"] = "";
    m_metadata["file_extension"] = "";
}
DocumentMetadata::~DocumentMetadata() = default;

string DocumentMetadata::routeImage(const filesystem::path &filePath)
{
    this->m_filePath = filePath.string();
    return this->m_filePath;
}

string DocumentMetadata::getAllMetadata(const filesystem::path &filePath)
{
    routeImage(filePath);
    readFileHeader();
    analyzeFileSignature();
    ostringstream oss;

    oss << "=== Document Metadata ===\n";
    oss << "File Name: " << getFileName() << "\n";
    oss << "File Type: " << getFileTypeExtension() << "\n";
    oss << "File Path: " << m_filePath << "\n";
    oss << "File Signature: " << getFileSignature() << "\n";
    oss << "MIME Type: " << getMimeType() << "\n";
    oss << "File Size: " << filesystem::file_size(m_filePath) << " bytes\n\n";

    return oss.str();
}

string DocumentMetadata::getFileSignature()
{
    auto it = m_metadata.find("signature");
    return (it != m_metadata.end()) ? it->second : "";
}

string DocumentMetadata::getMimeType()
{
    auto it = m_metadata.find("mime_type");
    return (it != m_metadata.end()) ? it->second : "";
}

string DocumentMetadata::getFileName() const
{
    auto it = m_metadata.find("file_name");
    return (it != m_metadata.end()) ? it->second : "";
}

string DocumentMetadata::getFileTypeExtension() const
{
    auto it = m_metadata.find("file_extension");
    return (it != m_metadata.end()) ? it->second : "";
}

void DocumentMetadata::readFileHeader()
{
    ifstream file(m_filePath, std::ios::binary);
    if (!file.is_open())
        return;

    // Leer primeros 256 bytes para análisis
    m_fileHeader.resize(256);
    file.read(reinterpret_cast<char *>(m_fileHeader.data()), 256);
    m_fileHeader.resize(file.gcount());

    file.close();
}

void DocumentMetadata::analyzeFileSignature()
{
    if (m_fileHeader.size() < 4)
        return;

    // Convertir primeros bytes a hex
    std::string signature = bytesToHex(std::vector<uint8_t>(m_fileHeader.begin(), m_fileHeader.begin() + 8));
    m_metadata["signature"] = signature;
    m_metadata["mime_type"] = detectMimeType();
    filesystem::path path(m_filePath);
    m_metadata["file_name"] = path.filename().string();
    m_metadata["file_extension"] = path.extension().string();
}

string DocumentMetadata::bytesToHex(const std::vector<uint8_t> &bytes) const
{
    std::ostringstream oss;
    for (uint8_t byte : bytes)
    {
        oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(byte);
    }
    return oss.str();
}

string DocumentMetadata::detectMimeType() const
{
    if (m_fileHeader.size() < 4)
        return "unknown";

    // PDF
    if (m_fileHeader[0] == 0x25 && m_fileHeader[1] == 0x50 &&
        m_fileHeader[2] == 0x44 && m_fileHeader[3] == 0x46)
    {
        return "application/pdf";
    }

    // EXE/PE
    if (m_fileHeader[0] == 0x4D && m_fileHeader[1] == 0x5A)
    {
        return "application/x-msdownload";
    }

    return "application/octet-stream";
}