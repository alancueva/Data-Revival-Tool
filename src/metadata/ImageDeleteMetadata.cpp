#include "../../include/metadata/ImageMetadata.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>

using namespace std;

bool ImageMetadata::cleanJPEGMetadata(const string &filepath, const string &tempFile)
{
    ifstream input(filepath, ios::binary);
    ofstream output(tempFile, ios::binary);

    if (!input.is_open() || !output.is_open())
        return false;

    /**
     * Leer y verificar el SOI marker (0xFFD8) (Start Of Image)
     *
     */
    unsigned char soi[2];
    input.read(reinterpret_cast<char *>(soi), 2);
    if (soi[0] != 0xFF || soi[1] != 0xD8)
        return false;
    output.write(reinterpret_cast<char *>(soi), 2);

    vector<char> segment_data_buffer;

    while (input.peek() != EOF)
    {
        unsigned char marker[2];
        input.read(reinterpret_cast<char *>(marker), 2);

        if (marker[0] != 0xFF)
        {
            // Estructura JPEG inválida o estamos dentro de los datos de imagen.
            // Por simplicidad, escribimos los bytes leídos y copiamos el resto.
            output.write(reinterpret_cast<char *>(marker), 2);
            output << input.rdbuf();
            break;
        }

        const unsigned char marker_code = marker[1];

        // Marcador EOI (End Of Image)
        if (marker_code == 0xD9)
        {
            output.write(reinterpret_cast<char *>(marker), 2);
            break;
        }

        // Marcadores standalone (sin campo de longitud)
        if ((marker_code >= 0xD0 && marker_code <= 0xD7) || marker_code == 0x01)
        {
            output.write(reinterpret_cast<char *>(marker), 2);
            continue;
        }

        // Leer la longitud del segmento
        unsigned char length_bytes[2];
        input.read(reinterpret_cast<char *>(length_bytes), 2);
        uint16_t segment_length = (length_bytes[0] << 8) | length_bytes[1];

        // Segmentos a eliminar (APPn, COM)
        // Se saltan los segmentos de metadatos
        if ((marker_code >= 0xE0 && marker_code <= 0xEF) || marker_code == 0xFE)
        {
            input.seekg(segment_length - 2, ios::cur);
        }
        else if (marker_code == 0xDA)
        {
            // Marcador SOS (Start Of Scan)
            // Escribir el marcador, la longitud y el encabezado del segmento
            output.write(reinterpret_cast<char *>(marker), 2);
            output.write(reinterpret_cast<char *>(length_bytes), 2);
            segment_data_buffer.resize(segment_length - 2);
            input.read(segment_data_buffer.data(), segment_length - 2);
            output.write(segment_data_buffer.data(), segment_length - 2);

            // Después de SOS, copiar el resto del flujo de datos de imagen
            output << input.rdbuf();
            break;
        }
        else
        {
            // Otros segmentos a conservar (DQT, DHT, SOF, etc.)
            output.write(reinterpret_cast<char *>(marker), 2);
            output.write(reinterpret_cast<char *>(length_bytes), 2);
            segment_data_buffer.resize(segment_length - 2);
            input.read(segment_data_buffer.data(), segment_length - 2);
            output.write(segment_data_buffer.data(), segment_length - 2);
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
    unsigned char signature[8];
    input.read(reinterpret_cast<char *>(signature), 8);

    /**
     * PNG Signature (8 bytes): 89 50 4E 47 0D 0A 1A 0A
     */
    const unsigned char pngSignature[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    if (memcmp(signature, pngSignature, 8) != 0)
        return false;

    // Escribir signature
    output.write(reinterpret_cast<char *>(signature), 8);

    // Conjunto de chunks a eliminar.
    // Los chunks críticos (IHDR, PLTE, IDAT, IEND) nunca deben estar aquí.
    const set<string> chunks_to_remove = {
        "tEXt", "zTXt", "iTXt", // Datos textuales
        "tIME",                 // Fecha de última modificación
        "pHYs",                 // Dimensiones físicas del píxel
        "sBIT",                 // Bits significativos
        "eXIf",                 // Metadatos EXIF
        "iCCP",                 // Perfil ICC embebido
        "sPLT",                 // Paleta sugerida
        "gAMA",                 // Corrección Gamma
        "cHRM"                  // Coordenadas de cromaticidad
    };
    vector<char> data_buffer;

    // Procesar chunks
    //
    while (input.good() && input.peek() != EOF)
    {
        unsigned char length_bytes[4];
        if (!input.read(reinterpret_cast<char *>(length_bytes), 4))
            break;

        uint32_t chunk_length = (length_bytes[0] << 24) | (length_bytes[1] << 16) |
                                (length_bytes[2] << 8) | length_bytes[3];

        char chunk_type_arr[4];
        input.read(chunk_type_arr, 4);
        if (input.fail())
            break;
        string typeStr(chunk_type_arr, 4);

        // Saltar chunks de metadatos
        // (tEXt, zTXt, iTXt, tIME, pHYs, sBIT, eXIf, iCCP, sPLT, gAMA, cHRM)
        // +4 para el CRC
        if (chunks_to_remove.count(typeStr))
        {
            input.seekg(chunk_length + 4, ios::cur);
            continue;
        }

        // Escribir chunk normal
        output.write(reinterpret_cast<char *>(length_bytes), 4);
        output.write(chunk_type_arr, 4);

        data_buffer.resize(chunk_length);
        input.read(data_buffer.data(), chunk_length);
        output.write(data_buffer.data(), chunk_length);

        char crc[4];
        input.read(crc, 4);
        output.write(crc, 4);

        // Si es IEND, terminar
        if (typeStr == "IEND")
            break;
    }
    return true;
}