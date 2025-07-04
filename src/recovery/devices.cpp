#include "../../include/recovery/devices.h"

#ifdef _WIN32
#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

std::vector<std::string> get_disks_windows()
{
    std::vector<std::string> drives;
    DWORD mask = GetLogicalDrives();

    for (char letter = 'A'; letter <= 'Z'; ++letter)
    {
        if (mask & (1 << (letter - 'A')))
        {
            std::string root_path = std::string(1, letter) + ":\\";
            UINT type = GetDriveTypeA(root_path.c_str());

            if (type == DRIVE_FIXED || type == DRIVE_REMOVABLE)
            {
                ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
                if (GetDiskFreeSpaceExA(root_path.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes))
                {
                    double size_gb = totalBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
                    std::ostringstream label;
                    label << root_path << " (" << std::fixed << std::setprecision(2) << size_gb << " GB)";
                    drives.push_back(label.str());
                }
                else
                {
                    drives.push_back(root_path + " (Tamaño desconocido)");
                }
            }
        }
    }

    return drives;
}

#elif __linux__
#include <cstdio>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdint>

std::vector<std::string> get_disks_linux()
{
    std::vector<std::string> result;
    FILE *fp = popen("lsblk -b -o NAME,SIZE,FSTYPE --noheadings", "r");
    if (!fp)
        return result;

    char buffer[256];

    while (fgets(buffer, sizeof(buffer), fp))
    {
        std::istringstream iss(buffer);
        std::string name, fstype;
        uint64_t size_bytes;

        if (!(iss >> name >> size_bytes)) continue;
        iss >> fstype;  

        /**
         * -b → muestra el tamaño en bytes exactos
         * convertimos a GB el resultado de size_bytes
         */
        double size_gb = size_bytes / (1024.0 * 1024.0 * 1024.0);

        /**
         * Excluimos dispositivos que no son relevantes
         * como dispositivos de bucle, zram o aquellos con tamaño menor a 10 MB
         * particiones de 0.00 GB o demasiado pequeñas (por ejemplo, de menos de 0.01 GB)
         */
        if (name.find("loop") == 0 || name.find("zram") == 0 || size_gb < 0.01)
            continue;

        std::ostringstream entry;
        entry << "/dev/" << name << " (" << std::fixed << std::setprecision(2) << size_gb << " GB)";

         if (!fstype.empty()) {
            entry << " [" << fstype << "]";
        }
        
        result.push_back(entry.str());
    }

    pclose(fp);
    return result;
}

#endif

std::vector<std::string> get_disks()
{
#ifdef _WIN32
    return get_disks_windows();
#elif __linux__
    return get_disks_linux();
#else
    return {};
#endif
}
