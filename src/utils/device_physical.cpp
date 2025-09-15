#include "../../include/utils/device_physical.h"

#include <regex>
#include <string>

namespace DevicePhysical {

/**
 * Verifica si el dispositivo es un disco físico válido (no partición)
 * Compatible con Windows y Linux
*/
bool is_physical_disk(const std::string& device_path) {
#ifdef _WIN32
    // En Windows los discos físicos tienen la forma: \\.\PhysicalDrive0
    std::regex disk_pattern(R"(\\\\\.\\PhysicalDrive[0-9]+)", std::regex::icase);
    return std::regex_match(device_path, disk_pattern);
#else
    if (device_path.empty()) return false;

    // Detectar NVMe particiones (terminan con p1, p2, etc.)
    std::regex nvme_part(R"(\/dev\/nvme\d+n\d+p\d+)");
    if (std::regex_match(device_path, nvme_part)) return false;

    // Detectar discos SATA/SCSI (termina en letra si es disco, número si es partición)
    char last_char = device_path.back();
    return !(last_char >= '0' && last_char <= '9');
#endif
}

}
