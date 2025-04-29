
#include <memory>
#include <string>
#include "../../include/metadata/FileMetadata.h"
#include "../../include/metadata/MetadataFactory.h"
using namespace std;

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


    /**
     * Realiza un análisis profundo del archivo cargado, extrayendo metadatos y analizando su historial de modificaciones.
     * 
     */
    std::string performDeepScan()
    {
        if (!fileLoaded)
            return "Ningún archivo cargado";

        std::string report = "== ANÁLISIS PROFUNDO ==\n\n";
        report += "Archivo: " + fileAnalyzer->getFilePath() + "\n";
        report += "Tipo: " + fileAnalyzer->getFileType() + "\n\n";

        report += "Metadatos actuales:\n";
        report += fileAnalyzer->extractMetadata() + "\n\n";

        report += "Metadatos recuperados:\n";
        report += fileAnalyzer->recoverOverwrittenMetadata() + "\n\n";


        report += "Análisis forense:\n";
        // report += "- Sectores analizados: " + std::to_string(fileAnalyzer->getAnalyzedSectors()) + "\n";
        // report += "- Bloques con metadatos residuales: " + std::to_string((fileAnalyzer->getResidualMetadataBlocks())) + "\n";
        // report += "- Historial de modificaciones detectado: " + std::to_string(fileAnalyzer->hasModificationHistory()) + "\n";
        // report += "- Secuencias de bytes recuperadas: " + fileAnalyzer->getRecoveredBytesSize() + "\n\n";

        // report += "Este archivo ha sido modificado aproximadamente " + std::to_string(fileAnalyzer->getModificationCount()) + " veces desde su creación.\n";
        // report += "Se han detectado " + std::to_string(fileAnalyzer->getEditingToolsCount()) +" herramientas diferentes usadas en su edición.\n";

        return report;
    }

    bool isFileLoaded() const
    {
        return fileLoaded;
    }
};