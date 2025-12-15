#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

class ImageMetadata {
public:
    ImageMetadata();
    ~ImageMetadata();

    struct ImageData {
        uint32_t width;
        uint32_t height;
        uint8_t bitDepth;
        uint8_t colorType;
        uint8_t compression;
        uint8_t filter;
        uint8_t interlace;
        double gamma;
        uint32_t pixelsPerUnitX;
        uint32_t pixelsPerUnitY;
        std::string pixelUnits;
        std::string srgbRendering;
    } imageData;

    std::string filePath;

    std::string setFilePath(const std::filesystem::path &filePath);

    bool extract(const std::filesystem::path &filePath);
    // string getAllMetadata() const;

    /**
     * @brief Extrae los metadatos de una imagen.
     * @param filePath Ruta del archivo de imagen.
     * @return string con los metadatos extraídos.
     * @note Este método utiliza la clase FileMetadata para extraer los metadatos de la imagen.
     */
    std::string extractMetadataImg(const std::filesystem::path &filePath);

    /**
     * Limpiar Metadata para JPEG
     */
    bool cleanJPEGMetadata(const std::string& filepath, const std::string& tempFile);

    /**
     * Limpiar Metadata para PNG
     */
    bool cleanPNGMetadata(const std::string& filepath, const std::string& tempFile);

private:
    std::ifstream fileStream;

    std::string getDimensionImage() const;
    std::string getFileType() const;

    std::string getPixelUnits() const;
    double getMegapixels() const; 
    std::string getFileName() const;
    std::string getFileTypeExtension() const;
    uint8_t getBitDepth() const;
    std::string getColorType() const;
    int64_t getFileSize() const;
    std::string getFormattedFileSize() const;


    int readJPEGBitDepth(std::ifstream &file) const;
    
    uint16_t readBigEndian16(std::ifstream& file) const;
};
#endif