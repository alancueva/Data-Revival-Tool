#include "../../include/metadata/DocumentMetadata.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <filesystem>

DocumentMetadata::DocumentMetadata() = default;
DocumentMetadata::~DocumentMetadata() = default;

std::string DocumentMetadata::extractMetadata(const std::filesystem::path &filePath) {
    if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
        return "Error: El archivo no existe o no es un archivo regular.";
    }

    m_filePath = filePath;
    parseFile();

    std::ostringstream oss;
    oss << "=== Metadatos del Documento ===\n\n";
    oss << "Nombre del archivo: " << m_metadata["file_name"] << "\n";
    oss << "Extensión: " << m_metadata["file_extension"] << "\n";
    oss << "Ruta: " << m_filePath.string() << "\n";
    oss << "Tamaño: " << formatFileSize(std::filesystem::file_size(m_filePath)) << "\n";
    oss << "Firma (Hex): " << m_metadata["signature"] << "\n";
    oss << "Tipo MIME: " << m_metadata["mime_type"] << "\n";
    oss << "Tipo de Documento: " << m_metadata["document_type"] << "\n\n";

    return oss.str();
}

void DocumentMetadata::parseFile() {
    readFileHeader();
    analyzeFile();
}

void DocumentMetadata::readFileHeader() {
    std::ifstream file(m_filePath, std::ios::binary);
    if (!file.is_open())
        return;

    // Leer primeros 256 bytes para análisis
    m_fileHeader.resize(256);
    file.read(reinterpret_cast<char*>(m_fileHeader.data()), 256);
    m_fileHeader.resize(file.gcount());
}

void DocumentMetadata::analyzeFile() {
    if (m_fileHeader.empty())
        return;

    m_metadata["file_name"] = m_filePath.filename().string();
    m_metadata["file_extension"] = m_filePath.extension().string();
    
    // Asegurarse de no leer más allá del final de la cabecera para la firma
    size_t signature_len = std::min(static_cast<size_t>(8), m_fileHeader.size());
    m_metadata["signature"] = bytesToHex(std::vector<uint8_t>(m_fileHeader.begin(), m_fileHeader.begin() + signature_len));
    
    detectDocumentType();
}

std::string DocumentMetadata::bytesToHex(const std::vector<uint8_t>& bytes) const {
    std::ostringstream oss;
    oss << std::hex << std::uppercase;
    for (uint8_t byte : bytes) {
        oss << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(byte);
    }
    return oss.str();
}

void DocumentMetadata::detectDocumentType() {
    // Si el archivo es muy pequeño para tener una firma, se marca como desconocido.
    if (m_fileHeader.size() < 4) {
        m_metadata["mime_type"] = "application/octet-stream";
        m_metadata["document_type"] = "Desconocido (archivo muy pequeño)";
        return;
    }

    // PDF: %PDF
    if (m_fileHeader[0] == 0x25 && m_fileHeader[1] == 0x50 && m_fileHeader[2] == 0x44 && m_fileHeader[3] == 0x46) { // %PDF
        m_metadata["mime_type"] = "application/pdf";
        m_metadata["document_type"] = "Documento PDF";
        return;
    }

    // Microsoft Office (DOCX, XLSX, PPTX) y OpenDocument (ODT, ODS, ODP) - Formato ZIP: PK..
    if (m_fileHeader[0] == 0x50 && m_fileHeader[1] == 0x4B && m_fileHeader[2] == 0x03 && m_fileHeader[3] == 0x04) { // PK..
        m_metadata["mime_type"] = "application/zip";
        // Para ser más específico, se necesitaría inspeccionar el contenido del archivo ZIP.
        m_metadata["document_type"] = "Documento Office (DOCX, XLSX, ODT, etc.)";
        return;
    }

    // Microsoft Office (DOC, XLS, PPT) - Formato OLE. Requiere 8 bytes.
    if (m_fileHeader.size() >= 8 &&
        m_fileHeader[0] == 0xD0 && m_fileHeader[1] == 0xCF && m_fileHeader[2] == 0x11 && m_fileHeader[3] == 0xE0 && // D0 CF 11 E0
        m_fileHeader[4] == 0xA1 && m_fileHeader[5] == 0xB1 && m_fileHeader[6] == 0x1A && m_fileHeader[7] == 0xE1) { // A1 B1 1A E1
        m_metadata["mime_type"] = "application/msword"; // Podría ser también xls, ppt
        m_metadata["document_type"] = "Documento MS Office 97-2003 (DOC, XLS, PPT)";
        return;
    }

    // Rich Text Format (RTF): {\rtf. Requiere 5 bytes.
    if (m_fileHeader.size() >= 5 &&
        m_fileHeader[0] == 0x7B && m_fileHeader[1] == 0x5C && m_fileHeader[2] == 0x72 && m_fileHeader[3] == 0x74 && m_fileHeader[4] == 0x66) { // {\rtf
        m_metadata["mime_type"] = "application/rtf";
        m_metadata["document_type"] = "Documento de Texto Enriquecido (RTF)";
        return;
    }

    // Ejecutable de Windows (PE): MZ
    if (m_fileHeader[0] == 0x4D && m_fileHeader[1] == 0x5A) { // MZ
        m_metadata["mime_type"] = "application/x-msdownload";
        m_metadata["document_type"] = "Ejecutable de Windows (EXE/DLL)";
        return;
    }

    // Por defecto
    m_metadata["mime_type"] = "application/octet-stream";
    m_metadata["document_type"] = "Tipo de archivo desconocido";
}

//! NOTE: Esta función está duplicada en `src/recovery/devices.cpp`. Considera moverla a un archivo de utilidades compartido.
std::string DocumentMetadata::formatFileSize(uint64_t bytes) const {
    const char* units[] = { "Bytes", "KB", "MB", "GB", "TB", "PB" };
    int unitIndex = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unitIndex < 5) { // Permitir hasta PB para consistencia
        size /= 1024.0;
        ++unitIndex;
    }

    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
    return out.str();
}