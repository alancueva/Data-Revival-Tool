#include "../../include/metadata/ImageMetadata.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <iostream>

using namespace std;


bool ImageMetadata::cleanJPEGMetadata(const string &filepath, const string &tempFile)
{
    ifstream input(filepath, ios::binary);
    ofstream output(tempFile, ios::binary);

    if (!input.is_open() || !output.is_open())
        return false;


    vector<unsigned char> buffer(2);

    // Leer y escribir SOI marker (0xFFD8)
    input.read(reinterpret_cast<char *>(buffer.data()), 2);
    if (buffer[0] != 0xFF || buffer[1] != 0xD8)
        return false;
    output.write(reinterpret_cast<char *>(buffer.data()), 2);

    while (input.read(reinterpret_cast<char *>(buffer.data()), 2))
    {
        if (buffer[0] == 0xFF)
        {
            // Verificar si es un segmento de metadatos
            if (buffer[1] == 0xE0 || buffer[1] == 0xE1 || buffer[1] == 0xE2 ||
                buffer[1] == 0xE3 || buffer[1] == 0xE4 || buffer[1] == 0xE5 ||
                buffer[1] == 0xE6 || buffer[1] == 0xE7 || buffer[1] == 0xE8 ||
                buffer[1] == 0xE9 || buffer[1] == 0xEA || buffer[1] == 0xEB ||
                buffer[1] == 0xEC || buffer[1] == 0xED || buffer[1] == 0xEE ||
                buffer[1] == 0xEF || buffer[1] == 0xFE)
            {

                // Leer longitud del segmento
                vector<unsigned char> lengthBytes(2);
                input.read(reinterpret_cast<char *>(lengthBytes.data()), 2);
                int segmentLength = (lengthBytes[0] << 8) | lengthBytes[1];

                // Saltar el resto del segmento de metadatos
                input.seekg(segmentLength - 2, ios::cur);
                continue;
            }

            // Si es SOS (Start of Scan) o EOI, escribir el resto del archivo
            if (buffer[1] == 0xDA || buffer[1] == 0xD9)
            {
                output.write(reinterpret_cast<char *>(buffer.data()), 2);
                output << input.rdbuf();
                break;
            }

            // Para otros marcadores, copiar normalmente
            output.write(reinterpret_cast<char *>(buffer.data()), 2);
        }
        else
        {
            // No es un marcador, retroceder y copiar el resto
            input.seekg(-1, ios::cur);
            output << input.rdbuf();
            break;
        }
    }
    return true;
}

bool ImageMetadata::cleanPNGMetadata(const string &filepath, const string &tempFile)
{
    ifstream input(filepath, ios::binary);
    ofstream output(tempFile, ios::binary);

    if (!input.is_open() || !output.is_open())
        return false;

    // Verificar signature PNG
    vector<unsigned char> signature(8);
    input.read(reinterpret_cast<char *>(signature.data()), 8);

    const unsigned char pngSignature[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    if (memcmp(signature.data(), pngSignature, 8) != 0)
        return false;

    // Escribir signature
    output.write(reinterpret_cast<char *>(signature.data()), 8);

    // Procesar chunks
    while (input.good())
    {
        vector<unsigned char> lengthBytes(4);
        if (!input.read(reinterpret_cast<char *>(lengthBytes.data()), 4))
            break;

        uint32_t chunkLength = (lengthBytes[0] << 24) | (lengthBytes[1] << 16) |
                               (lengthBytes[2] << 8) | lengthBytes[3];

        vector<unsigned char> chunkType(4);
        input.read(reinterpret_cast<char *>(chunkType.data()), 4);

        string typeStr(chunkType.begin(), chunkType.end());

        // Saltar chunks de metadatos
        if (typeStr == "tEXt" || typeStr == "zTXt" || typeStr == "iTXt" ||
            typeStr == "tIME" || typeStr == "pHYs" || typeStr == "sBIT" ||
            typeStr == "eXIf")
        {
            input.seekg(chunkLength + 4, ios::cur); // +4 para CRC
            continue;
        }

        // Escribir chunk normal
        output.write(reinterpret_cast<char *>(lengthBytes.data()), 4);
        output.write(reinterpret_cast<char *>(chunkType.data()), 4);

        // Copiar datos del chunk
        vector<unsigned char> chunkData(chunkLength);
        input.read(reinterpret_cast<char *>(chunkData.data()), chunkLength);
        output.write(reinterpret_cast<char *>(chunkData.data()), chunkLength);

        // Copiar CRC
        vector<unsigned char> crc(4);
        input.read(reinterpret_cast<char *>(crc.data()), 4);
        output.write(reinterpret_cast<char *>(crc.data()), 4);

        // Si es IEND, terminar
        if (typeStr == "IEND")
            break;
    }
    return true;
}