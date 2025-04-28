
#include <memory>
#include <string>
#include "../../include/metadata/FileMetadata.h"
#include "../../include/metadata/MetadataFactory.h"

class MetadataRecoveryEngine
{
private:
    std::unique_ptr<FileMetadata> fileAnalyzer;
    bool fileLoaded;
    std::string currentFilePath;

public:
    MetadataRecoveryEngine() : fileLoaded(false) {}

    bool loadFile(const std::string &filePath)
    {
        currentFilePath = filePath;
        fileAnalyzer = MetadataFactory::createAnalyzer(filePath);

        if (!fileAnalyzer)
        {
            fileLoaded = false;
            return false;
        }

        fileLoaded = fileAnalyzer->analyze();
        return fileLoaded;
    }

    std::string getFileType() const
    {
        if (!fileLoaded)
            return "Ningún archivo cargado";
        return fileAnalyzer->getFileType();
    }

    std::string getBasicMetadata()
    {
        if (!fileLoaded)
            return "Ningún archivo cargado";
        return fileAnalyzer->extractMetadata();
    }

    std::string recoverDeletedMetadata()
    {
        if (!fileLoaded)
            return "Ningún archivo cargado";
        return fileAnalyzer->recoverOverwrittenMetadata();
    }

    std::string performDeepScan()
    {
        if (!fileLoaded)
            return "Ningún archivo cargado";

        // Simulación de un escaneo profundo
        std::string report = "== ANÁLISIS PROFUNDO ==\n\n";
        report += "Archivo: " + fileAnalyzer->getFilePath() + "\n";
        report += "Tipo: " + fileAnalyzer->getFileType() + "\n\n";

        report += "Metadatos actuales:\n";
        report += fileAnalyzer->extractMetadata() + "\n\n";

        report += "Metadatos recuperados:\n";
        report += fileAnalyzer->recoverOverwrittenMetadata() + "\n\n";

        report += "Análisis forense:\n";
        report += "- Sectores analizados: 245\n";
        report += "- Bloques con metadatos residuales: 17\n";
        report += "- Historial de modificaciones detectado\n";
        report += "- Secuencias de bytes recuperadas: 3.4 KB\n\n";

        report += "Este archivo ha sido modificado aproximadamente 8 veces desde su creación.\n";
        report += "Se han detectado 3 herramientas diferentes usadas en su edición.\n";

        return report;
    }

    bool isFileLoaded() const
    {
        return fileLoaded;
    }
};