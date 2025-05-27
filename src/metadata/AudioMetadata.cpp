#include "../../include/metadata/AudioMetadata.h"
#include <fstream>
#include <sstream>
using namespace std;

AudioMetadata::AudioMetadata() {
    filePath = "";
}

AudioMetadata::~AudioMetadata(){
    m_fileHeader.clear();
    m_metadata.clear();
};

string AudioMetadata::routeAudio(const filesystem::path &filePath) {
    this->filePath = filePath.string();
    return this->filePath;
}

string AudioMetadata::getAllMetadata(const filesystem::path &filePath) {
    routeAudio(filePath);
    readFileHeader();
    analyzeAudioFormat();
    ostringstream oss;
    oss << "=== Audio Metadata ===\n";
    oss << "File Path: " << filePath.string() << "\n";
    oss << "Format: " << getFormat() << "\n";
    oss << "Title: " << getTitle() << "\n";
    oss << "Artist: " << getArtist() << "\n";
    oss << "Album: " << getAlbum() << "\n";
    oss << "Duration: " << getDuration() << "\n";
    oss << "Bitrate: " << getBitrate() << "\n";
    oss << "Sample Rate: " << getSampleRate() << "\n";
    oss << "Channels: " << getChannels() << "\n";
    return oss.str();
}

string AudioMetadata::getTitle() const {
    auto it = m_metadata.find("title");
    return (it != m_metadata.end()) ? it->second : "Unknown";
}

string AudioMetadata::getDuration() const {
    auto it = m_metadata.find("duration");
    return (it != m_metadata.end()) ? it->second : "Unknown";
}

string AudioMetadata::getArtist() const {
    auto it = m_metadata.find("artist");
    return (it != m_metadata.end()) ? it->second : "Unknown";
}

string AudioMetadata::getAlbum() const {
    auto it = m_metadata.find("album");
    return (it != m_metadata.end()) ? it->second : "Unknown";
}

string AudioMetadata::getBitrate() const {
    auto it = m_metadata.find("bitrate");
    return (it != m_metadata.end()) ? it->second : "Unknown";
}

string AudioMetadata::getSampleRate() const {
    auto it = m_metadata.find("sample_rate");
    return (it != m_metadata.end()) ? it->second : "Unknown";
}

string AudioMetadata::getFormat() const {
    auto it = m_metadata.find("format");
    return (it != m_metadata.end()) ? it->second : "Unknown";
}

string AudioMetadata::getChannels() const {
    auto it = m_metadata.find("channels");
    return (it != m_metadata.end()) ? it->second : "Unknown";
}

/**
 * @brief Método para detectar el formato de audio basado en la firma del archivo.
 * @note Este método analiza los primeros bytes del archivo para determinar su formato.
 */
void AudioMetadata::readFileHeader() {
    ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) return;

    // Leer primeros 256 bytes para análisis
    m_fileHeader.resize(256);
    file.read(reinterpret_cast<char *>(m_fileHeader.data()), 256);
    m_fileHeader.resize(file.gcount());

    file.close();
}


void AudioMetadata::analyzeAudioFormat(){
    // if (m_fileHeader.size() < 12) return;

    string format = detectAudioFormat();
    m_metadata["format"] = format;

    if (format == "MP3") {
        extractMP3Metadata();
        extractID3Tags();
    } else if (format == "WAV") {
        // extractWAVMetadata();
    } else {
        
    }
}

string AudioMetadata:: detectAudioFormat() {
    // if (m_fileHeader.size() < 12) return;

    if ((m_fileHeader[0] == 0xFF && (m_fileHeader[1] & 0xE0) == 0xE0) ||
        (m_fileHeader[0] == 'I' && m_fileHeader[1] == 'D' && m_fileHeader[2] == '3')) {
        return "MP3";
    }
    

    return "";
}

void AudioMetadata::extractMP3Metadata() {
    // Buscar frame header MP3
    for (size_t i = 0; i < m_fileHeader.size() - 4; ++i) {
        if (m_fileHeader[i] == 0xFF && (m_fileHeader[i + 1] & 0xE0) == 0xE0) {
            uint8_t header[4] = {m_fileHeader[i], m_fileHeader[i + 1], 
                                m_fileHeader[i + 2], m_fileHeader[i + 3]};
            
            // Extraer información del frame header
            int version = (header[1] >> 3) & 0x03;
            int layer = (header[1] >> 1) & 0x03;
            int bitrate_index = (header[2] >> 4) & 0x0F;
            int sample_rate_index = (header[2] >> 2) & 0x03;
            int channel_mode = (header[3] >> 6) & 0x03;
            
            // Tabla de bitrates para MPEG-1 Layer III
            int bitrates[] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0};
            int sample_rates[] = {44100, 48000, 32000, 0};
            
            if (bitrate_index > 0 && bitrate_index < 15) {
                m_metadata["bitrate"] = to_string(bitrates[bitrate_index]) + " kbps";
            }
            
            if (sample_rate_index < 3) {
                m_metadata["sample_rate"] = to_string(sample_rates[sample_rate_index]) + " Hz";
            }
            
            m_metadata["channels"] = (channel_mode == 3) ? "1 (Mono)" : "2 (Stereo)";
            
            // Calcular duración aproximada
            if (bitrate_index > 0 && bitrate_index < 15) {
                auto file_size = filesystem::file_size(filePath);
                uint32_t duration_seconds = (file_size * 8) / (bitrates[bitrate_index] * 1000);
                m_metadata["duration"] = formatDuration(duration_seconds);
            }
            
            break;
        }
    }
}

string AudioMetadata::formatDuration(uint32_t seconds) const {
    uint32_t hours = seconds / 3600;
    uint32_t minutes = (seconds % 3600) / 60;
    uint32_t secs = seconds % 60;
    
    ostringstream oss;
    if (hours > 0) {
        oss << hours << ":";
    }
    oss << setfill('0') << setw(2) << minutes << ":"
        << setfill('0') << setw(2) << secs;

    return oss.str();
}



void AudioMetadata::extractID3Tags() {
    // Buscar tags ID3v2
    if (m_fileHeader.size() > 10 && 
        m_fileHeader[0] == 'I' && m_fileHeader[1] == 'D' && m_fileHeader[2] == '3') {
        
        // Buscar frames comunes
        for (size_t i = 10; i < m_fileHeader.size() - 10; ++i) {
            // TIT2 (Title)
            if (i + 4 < m_fileHeader.size() &&
                m_fileHeader[i] == 'T' && m_fileHeader[i + 1] == 'I' &&
                m_fileHeader[i + 2] == 'T' && m_fileHeader[i + 3] == '2') {
                
                uint32_t frame_size = readUInt32BE(i + 4);
                if (frame_size > 0 && frame_size < 256 && i + 11 + frame_size < m_fileHeader.size()) {
                    string title = readString(i + 11, frame_size - 1);
                    if (!title.empty()) {
                        m_metadata["title"] = title;
                    }
                }
            }
            
            // TPE1 (Artist)
            if (i + 4 < m_fileHeader.size() &&
                m_fileHeader[i] == 'T' && m_fileHeader[i + 1] == 'P' &&
                m_fileHeader[i + 2] == 'E' && m_fileHeader[i + 3] == '1') {
                
                uint32_t frame_size = readUInt32BE(i + 4);
                if (frame_size > 0 && frame_size < 256 && i + 11 + frame_size < m_fileHeader.size()) {
                    string artist = readString(i + 11, frame_size - 1);
                    if (!artist.empty()) {
                        m_metadata["artist"] = artist;
                    }
                }
            }
            
            // TALB (Album)
            if (i + 4 < m_fileHeader.size() &&
                m_fileHeader[i] == 'T' && m_fileHeader[i + 1] == 'A' &&
                m_fileHeader[i + 2] == 'L' && m_fileHeader[i + 3] == 'B') {
                
                uint32_t frame_size = readUInt32BE(i + 4);
                if (frame_size > 0 && frame_size < 256 && i + 11 + frame_size < m_fileHeader.size()) {
                    string album = readString(i + 11, frame_size - 1);
                    if (!album.empty()) {
                        m_metadata["album"] = album;
                    }
                }
            }
        }
    }
}

string AudioMetadata::readString(size_t offset, size_t length) const {
    if (offset + length > m_fileHeader.size()) return "";
    
    string result;
    for (size_t i = 0; i < length; ++i) {
        char c = static_cast<char>(m_fileHeader[offset + i]);
        if (c == 0) break;
        if (c >= 32 && c < 127) {
            result += c;
        }
    }
    return result;
}

uint32_t AudioMetadata::readUInt32BE(size_t offset) const {
    if (offset + 4 > m_fileHeader.size()) return 0;
    return (m_fileHeader[offset] << 24) | (m_fileHeader[offset + 1] << 16) |
           (m_fileHeader[offset + 2] << 8) | m_fileHeader[offset + 3];
}