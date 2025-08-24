#include "../../include/metadata/AudioMetadata.h"
#include <fstream>
#include <sstream>
#include <iomanip>

// Los miembros (map, vector, path) se inicializan y limpian automáticamente.
AudioMetadata::AudioMetadata() = default;
AudioMetadata::~AudioMetadata() = default;

std::string AudioMetadata::getAllMetadata(const std::filesystem::path &filePath)
{
    // Limpiar estado anterior para reutilizar el objeto
    m_fileHeader.clear();
    m_metadata.clear();
    m_filePath = filePath;

    readFileHeader();
    analyzeAudioFormat();

    std::ostringstream oss;
    oss << "=== Audio Metadata ===\n";
    oss << "File Path: " << filePath.string() << "\n";
    oss << "Format: " << getFormat() << "\n";
    oss << "Title: " << getTitle() << "\n";
    oss << "Artist: " << getArtist() << "\n";
    oss << "Album: " << getAlbum() << "\n";
    oss << "Bitrate: " << getBitrate() << "\n";
    oss << "Sample Rate: " << getSampleRate() << "\n";
    oss << "Channels: " << getChannels() << "\n";
    oss << "Duration: " << getDuration() << "\n";
    return oss.str();
}

std::string AudioMetadata::getMetadataValue(const std::string &key) const
{
    auto it = m_metadata.find(key);
    return (it != m_metadata.end()) ? it->second : "Unknown";
}

std::string AudioMetadata::getTitle() const
{
    return getMetadataValue("title");
}

std::string AudioMetadata::getDuration() const
{
    return getMetadataValue("duration");
}

std::string AudioMetadata::getArtist() const
{
    return getMetadataValue("artist");
}

std::string AudioMetadata::getAlbum() const
{
    return getMetadataValue("album");
}

std::string AudioMetadata::getBitrate() const
{
    return getMetadataValue("bitrate");
}

std::string AudioMetadata::getSampleRate() const
{
    return getMetadataValue("sample_rate");
}

std::string AudioMetadata::getFormat() const
{
    return getMetadataValue("format");
}

std::string AudioMetadata::getChannels() const
{
    return getMetadataValue("channels");
}

void AudioMetadata::readFileHeader()
{
    std::ifstream file(m_filePath, std::ios::binary);
    if (!file.is_open())
        return;

    // Leer primeros 4KB para análisis. 256 bytes es muy poco si hay una etiqueta ID3 grande (ej. con carátula).
    const size_t header_size_to_read = 4096;
    m_fileHeader.resize(header_size_to_read);
    file.read(reinterpret_cast<char *>(m_fileHeader.data()), header_size_to_read);
    m_fileHeader.resize(file.gcount());
}

void AudioMetadata::analyzeAudioFormat()
{
    std::string format = detectAudioFormat();
    m_metadata["format"] = format;

    if (format == "MP3")
    {
        extractID3Tags();
        extractMP3Metadata();
    }
    // Se pueden añadir más formatos aquí (AAC, FLAC, etc.)
}

std::string AudioMetadata::detectAudioFormat()
{
    if (m_fileHeader.size() < 3)
    {
        return "Unknown";
    }

    // MP3 puede empezar con un frame sync (FF FB..) o con una tag ID3
    if ((m_fileHeader[0] == 0xFF && (m_fileHeader[1] & 0xE0) == 0xE0) ||
        (m_fileHeader[0] == 'I' && m_fileHeader[1] == 'D' && m_fileHeader[2] == '3'))
    {
        return "MP3";
    }

    return "Unknown";
}

void AudioMetadata::extractMP3Metadata()
{
    // Buscar frame header MP3
    for (size_t i = 0; i < m_fileHeader.size() - 4; ++i)
    {
        if (m_fileHeader[i] == 0xFF && (m_fileHeader[i + 1] & 0xE0) == 0xE0)
        {
            uint8_t header[4] = {m_fileHeader[i], m_fileHeader[i + 1],
                                 m_fileHeader[i + 2], m_fileHeader[i + 3]};

            // Extraer información del frame header
            int version = (header[1] >> 3) & 0x03;
            int layer = (header[1] >> 1) & 0x03;
            int bitrate_index = (header[2] >> 4) & 0x0F;
            int sample_rate_index = (header[2] >> 2) & 0x03;
            int channel_mode = (header[3] >> 6) & 0x03;

            // extraídos del encabezado del frame MP3.
            // Los valores están definidos en la especificación MPEG.
            // El primer y último '0' en el array de bitrates son para manejar índices no válidos o reservados.
            // El último '0' en el array de sample_rates es para manejar un índice reservado.
            int bitrates[] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0};
            // Estos arrays contienen los valores de bitrate y sample rate correspondientes a los índices
            int sample_rates[] = {44100, 48000, 32000, 0}; 

            if (bitrate_index > 0 && bitrate_index < 15)
            {
                m_metadata["bitrate"] = std::to_string(bitrates[bitrate_index]) + " kbps";
            }

            if (sample_rate_index < 3)
            {
                m_metadata["sample_rate"] = std::to_string(sample_rates[sample_rate_index]) + " Hz";
            }

            m_metadata["channels"] = (channel_mode == 3) ? "1 (Mono)" : "2 (Stereo)";

            // Calcular duración aproximada (funciona mejor para CBR)
            // Solo calcular si no se encontró una duración más precisa en las etiquetas ID3.
            if (m_metadata.find("duration") == m_metadata.end())
            {
                if (bitrate_index > 0 && bitrate_index < 15)
                {
                    uint64_t file_size = std::filesystem::file_size(m_filePath);
                    uint64_t duration_seconds = (file_size * 8) / (static_cast<uint64_t>(bitrates[bitrate_index]) * 1000);
                    m_metadata["duration"] = formatDuration(static_cast<uint32_t>(duration_seconds));
                }
            }

            break;
        }
    }
}

std::string AudioMetadata::formatDuration(uint32_t seconds) const
{
    uint32_t hours = seconds / 3600;
    uint32_t minutes = (seconds % 3600) / 60;
    uint32_t secs = seconds % 60;

    std::ostringstream oss;
    if (hours > 0)
    {
        oss << hours << ":";
    }
    oss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << secs;

    return oss.str();
}

void AudioMetadata::extractID3Tags()
{
    /**
     * Verifica si el archivo tiene una cabecera ID3v2 válida.
     * @return true si es un archivo ID3v2, false en caso contrario.
     */
    if (m_fileHeader.size() < 10 || m_fileHeader[0] != 'I' || m_fileHeader[1] != 'D' || m_fileHeader[2] != '3')
    {
        return;
    }

    // El tamaño de la etiqueta ID3v2 es un entero "synchsafe" de 28 bits.
    uint32_t tag_size = (m_fileHeader[6] << 21) | (m_fileHeader[7] << 14) | (m_fileHeader[8] << 7) | m_fileHeader[9];

    size_t pos = 10; // Empezar a leer frames después de la cabecera ID3 de 10 bytes
    size_t limit = std::min(static_cast<size_t>(tag_size + 10), m_fileHeader.size());

    /**
     * Lee los frames ID3v2 y extrae los metadatos relevantes.
     * @note Este método asume que los frames están codificados en UTF-8.
     * Recorrer los frames de la etiqueta ID3 de forma estructurada.
     */
    while (pos < limit - 10)
    {
        std::string frame_id(reinterpret_cast<const char *>(&m_fileHeader[pos]), 4);
        uint32_t frame_size = readUInt32BE(pos + 4);

        // Si el ID del frame empieza con null o el tamaño es 0, hemos llegado al padding.
        if (frame_id[0] == 0 || frame_size == 0)
        {
            break;
        }

        /**
         * Offset del contenido del frame.
         * Contenido del frame (después de la cabecera del frame de 10 bytes)
         * El frame se sale de los límites de la etiqueta que hemos leído
         */
        size_t frame_content_offset = pos + 10;
        if (frame_content_offset + frame_size > limit)
        {
            break;
        }

        if (frame_id == "TIT2")
        { 
            // Título
            std::string title = readString(frame_content_offset + 1, frame_size - 1); // +1 para saltar el byte de codificación
            if (!title.empty())
                m_metadata["title"] = title;
        }
        else if (frame_id == "TPE1")
        { 
            // Artista
            std::string artist = readString(frame_content_offset + 1, frame_size - 1);
            if (!artist.empty())
                m_metadata["artist"] = artist;
        }
        else if (frame_id == "TALB")
        { 
            // Álbum
            std::string album = readString(frame_content_offset + 1, frame_size - 1);
            if (!album.empty())
                m_metadata["album"] = album;
        }
        else if (frame_id == "TLEN")
        { 
            // Duración en ms
            std::string length_ms_str = readString(frame_content_offset + 1, frame_size - 1);
            if (!length_ms_str.empty())
            {
                try
                {
                    uint64_t duration_ms = std::stoull(length_ms_str);
                    if (duration_ms > 0)
                    {
                        m_metadata["duration"] = formatDuration(duration_ms / 1000);
                    }
                }
                catch (const std::exception &)
                {
                    // Ignorar si el valor no es un número válido
                }
            }
        }

        // Moverse al siguiente frame
        pos += 10 + frame_size;
    }
}

std::string AudioMetadata::readString(size_t offset, size_t length) const
{
    if (offset + length > m_fileHeader.size())
        return "";

    // NOTE: Esta implementación asume una codificación simple ASCII/ISO-8859-1.
    // Una implementación completa de ID3 necesitaría verificar el byte de codificación.
    std::string result;
    for (size_t i = 0; i < length; ++i)
    {
        char c = static_cast<char>(m_fileHeader[offset + i]);
        if (c == 0)
            break;
        result += c;
    }
    return result;
}

uint32_t AudioMetadata::readUInt32BE(size_t offset) const
{
    if (offset + 4 > m_fileHeader.size())
        return 0;
    return (m_fileHeader[offset] << 24) | (m_fileHeader[offset + 1] << 16) |
           (m_fileHeader[offset + 2] << 8) | m_fileHeader[offset + 3];
}