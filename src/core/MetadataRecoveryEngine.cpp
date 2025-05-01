
#include "../../include/metadata/FileMetadata.h"
#include "../../include/metadata/MetadataFactory.h"
#include "../../include/core/MetadataRecoveryEngine.h"
#include <string>
#include <memory>
using namespace std;

MetadataRecoveryEngine::MetadataRecoveryEngine() : fileLoaded(false) 
{

}


bool MetadataRecoveryEngine::loadFile(const string &filePath)
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

string MetadataRecoveryEngine::getFileType() const
{
    if (!fileLoaded)
        return "Ningún archivo cargado";
    return fileAnalyzer->getFileType();
}

string MetadataRecoveryEngine::getBasicMetadata() const
{
    if (!fileLoaded)
        return "Ningún archivo cargado";
    return fileAnalyzer->extractMetadata();
}

string MetadataRecoveryEngine::recoverDeletedMetadata()
{
    if (!fileLoaded)
        return "Ningún archivo cargado";
    return fileAnalyzer->recoverOverwrittenMetadata();
}

string MetadataRecoveryEngine::performDeepScan()
{
    if (!fileLoaded)
        return "Ningún archivo cargado";

    string report = "== ANÁLISIS PROFUNDO ==\n\n";
    report += "Archivo: " + fileAnalyzer->getFilePath() + "\n";
    report += "Tipo: " + fileAnalyzer->getFileType() + "\n\n";

    report += "Metadatos actuales:\n";
    report += fileAnalyzer->extractMetadata() + "\n\n";

    report += "Metadatos recuperados:\n";
    report += fileAnalyzer->recoverOverwrittenMetadata() + "\n\n";

    report += "Análisis forense:\n";
  

    return report;
}

bool MetadataRecoveryEngine::isFileLoaded() const
{
    return fileLoaded;
}
