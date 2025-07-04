#include "../../include/recovery/devices.h"

#ifdef _WIN32
#include <windows.h>

std::vector<std::string> get_disks_windows()
{
    std::vector<std::string> drives;
    DWORD mask = GetLogicalDrives();

    for (char letter = 'A'; letter <= 'Z'; ++letter)
    {
        if (mask & (1 << (letter - 'A')))
        {
            std::string drive = std::string(1, letter) + ":\\";
            UINT type = GetDriveTypeA(drive.c_str());
            if (type == DRIVE_FIXED || type == DRIVE_REMOVABLE)
            {
                drives.push_back(drive);
            }
        }
    }

    return drives;
}

#elif __linux__
#include <fstream>
#include <sstream>
#include <regex>


std::vector<std::string> get_disks_linux()
{
    std::vector<std::string> devices;
    std::ifstream file("/proc/partitions");
    std::string line;
    std::regex disk_regex("^[a-z]+$");  // solo letras, sin número final

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string major, minor, blocks, name;
        if (!(iss >> major >> minor >> blocks >> name)) continue;

        // if (name.find("sd") == 0 || name.find("nvme") == 0)
        // {
        //     devices.push_back("/dev/" + name);
        // }

        // Solo agregar los discos (no particiones)
         if ((name.find("sd") == 0 || name.find("nvme") == 0) && std::regex_match(name, disk_regex)) {
            devices.push_back("/dev/" + name);
        }
    }

    return devices;
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
