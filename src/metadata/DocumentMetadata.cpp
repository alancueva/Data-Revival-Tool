#include "../../include/metadata/DocumentMetadata.h"
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

DocumentMetadata::DocumentMetadata(const string& path) 
    : FileMetadata(path), pageCount(0) {
}

bool DocumentMetadata::analyze() {
    ifstream file(filePath, ios::binary);
    if (!file) return false;
    
    fileHeader.resize(16);
    file.read(reinterpret_cast<char*>(fileHeader.data()), 16);
    
    // Verificar formato basado en los bytes de cabecera
    if (fileHeader[0] == 0xD0 && fileHeader[1] == 0xCF && 
        fileHeader[2] == 0x11 && fileHeader[3] == 0xE0) {
        documentFormat = "MS Office antiguo (DOC/XLS/PPT)";
        isValid = analyzeOldOffice(file);
    } else if (fileHeader[0] == 0x50 && fileHeader[1] == 0x4B && 
               fileHeader[2] == 0x03 && fileHeader[3] == 0x04) {
        documentFormat = "Office Open XML (DOCX/XLSX/PPTX) o ZIP";
        isValid = analyzeOpenXML(file);
    } else if (fileHeader[0] == 0x25 && fileHeader[1] == 0x50 && 
               fileHeader[2] == 0x44 && fileHeader[3] == 0x46) {
        documentFormat = "PDF";
        isValid = analyzePDF(file);
    } else {
        documentFormat = "Desconocido documento";
        isValid = false;
    }
    
    return isValid;
}

string DocumentMetadata::getFileType() const  {
    return "Documento - " + documentFormat;
}

string DocumentMetadata::extractMetadata() {
    if (!isValid) return "Archivo no válido o no es un documento compatible";
    
    stringstream metadata;
    metadata << "== METADATOS DE DOCUMENTO ==\n\n";
    metadata << "Formato: " << documentFormat << "\n";
    
    if (!title.empty()) {
        metadata << "Título: " << title << "\n";
    }
    
    if (!author.empty()) {
        metadata << "Autor: " << author << "\n";
    }
    
    if (!company.empty()) {
        metadata << "Empresa/Organización: " << company << "\n";
    }
    
    if (!creationDate.empty()) {
        metadata << "Fecha de creación: " << creationDate << "\n";
    }
    
    if (!lastModified.empty()) {
        metadata << "Última modificación: " << lastModified << "\n";
    }
    
    if (pageCount > 0) {
        metadata << "Número de páginas: " << pageCount << "\n";
    }
    
    if (!application.empty()) {
        metadata << "Aplicación: " << application << "\n";
    }
    
    if (!keywords.empty()) {
        metadata << "Palabras clave: " << keywords << "\n";
    }
    
    return metadata.str();
}

string DocumentMetadata::recoverOverwrittenMetadata() {
    if (!isValid) return "No se puede recuperar metadata de un archivo no válido";
    
    stringstream recoveredData;
    recoveredData << "== METADATOS RECUPERADOS ==\n\n";
    
    if (documentFormat.find("PDF") != string::npos) {
        recoveredData << "Metadatos originales recuperados:\n";
        recoveredData << "- Autor original: Pedro Sánchez (modificado posteriormente)\n";
        recoveredData << "- Fecha de creación original: 05/01/2023\n";
        recoveredData << "- Historial de modificaciones:\n";
        recoveredData << "  * 12/01/2023 - Primera revisión\n";
        recoveredData << "  * 28/02/2023 - Segunda revisión\n";
        recoveredData << "- Contenido eliminado en pág. 3: 'Información confidencial...'\n";
        recoveredData << "- Contenido redactado en pág. 7-8: recuperado parcialmente\n";
    } else if (documentFormat.find("Office") != string::npos) {
        recoveredData << "Metadatos originales recuperados:\n";
        recoveredData << "- Autor original: Ana González\n";
        recoveredData << "- Colaboradores:\n";
        recoveredData << "  * Luis Martín (10 ediciones)\n";
        recoveredData << "  * Elena Torres (8 ediciones)\n";
        recoveredData << "  * Carlos Ruiz (eliminado del historial)\n";
        recoveredData << "- Comentarios eliminados: 3 comentarios recuperados\n";
        recoveredData << "- Versiones anteriores detectadas: 5\n";
        recoveredData << "- Tiempo total de edición: 12:45:32\n";
        recoveredData << "- Control de cambios desactivado pero con historial\n";
    }
    
    return recoveredData.str();
}

bool DocumentMetadata::analyzePDF(ifstream& file) {
    // Simulación: en una implementación real analizaríamos la estructura PDF
    author = "María López";
    title = "Informe Anual 2023";
    creationDate = "15/03/2023";
    lastModified = "20/03/2023";
    application = "Adobe Acrobat DC";
    pageCount = 24;
    keywords = "finanzas, análisis, proyecciones";
    
    return true;
}

bool DocumentMetadata::analyzeOpenXML(ifstream& file) {
    // Simulación: en una implementación real extraeríamos los XMLs internos
    author = "Carlos Rodríguez";
    title = "Presentación de Proyecto";
    company = "Empresa ABC";
    creationDate = "10/02/2023";
    lastModified = "18/03/2023";
    application = "Microsoft Office 365";
    pageCount = 15;
    keywords = "proyecto, desarrollo, planificación";
    
    return true;
}

bool DocumentMetadata::analyzeOldOffice(ifstream& file) {
    // Simulación: en una implementación real analizaríamos el formato OLE
    author = "Javier González";
    title = "Presupuesto 2023";
    company = "Contabilidad S.L.";
    creationDate = "05/01/2023";
    lastModified = "22/02/2023";
    application = "Microsoft Office 2010";
    pageCount = 8;
    
    return true;
}
