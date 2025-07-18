#include "../../include/recovery/devices.h"

#ifdef _WIN32
#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <winioctl.h>
#include <map>

/**
 * Función para formatear el tamaño en una cadena legible
 * Ejemplo: 1024 bytes → "1.00 KB", 1048576 bytes → "1.00 MB"
 * Utiliza un formato de dos decimales y unidades   
 * Bytes, KB, MB, GB, TB, PB
 * @param bytes Tamaño en bytes
 * @return Cadena formateada con el tamaño y la unidad
 */
std::string format_size(ULONGLONG bytes)
{
    const char* units[] = { "Bytes", "KB", "MB", "GB", "TB", "PB" };
    int unitIndex = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unitIndex < 5) {  // < 5 para permitir hasta PB
        size /= 1024.0;
        ++unitIndex;
    }

    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
    return out.str();
}

/**
 * Función para obtener las unidades de disco en Windows
 * Filtra unidades fijas y extraíbles, excluyendo unidades de red, CD-ROM y
 * unidades de disco extraíbles que no son relevantes.
 * @return Vector de cadenas con las unidades de disco encontradas
 */
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

            /**
             * Filtramos las unidades que no son relevantes
             * como unidades de red, CD-ROM o unidades de disco extraíbles
             * DRIVE_FIXED → unidades fijas (discos duros)
             * DRIVE_REMOVABLE → unidades extraíbles (pendrives, tarjetas SD, etc
             */
            if (type == DRIVE_FIXED || type == DRIVE_REMOVABLE)
            {
                ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
                if (GetDiskFreeSpaceExA(root_path.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes))
                {
                    /**
                     * Formateamos el tamaño total de la unidad
                     * y lo añadimos al vector de unidades
                     * Utilizamos una cadena de salida para formatear el tamaño
                     * Ejemplo: "C:\ (500.00 GB)"
                     * Si no se puede obtener el tamaño, añadimos un mensaje de "Tamaño desconocido"
                     */
                    std::ostringstream label;
                    label << root_path << " (" << format_size(totalBytes.QuadPart) << ")";
                    drives.push_back(label.str());
                }
                else
                {
                    drives.push_back(root_path + " (Tamaño desconocido)");
                }
            }
            else if (type == DRIVE_NO_ROOT_DIR)
            {
                /*
                * Maneja casos especiales donde Windows no puede acceder a la raíz
                * de la unidad, como unidades de red desconectadas o unidades no montadas.
                * En este caso, simplemente añadimos la unidad con un mensaje de acceso denegado
                */
                drives.push_back(root_path + " (Acceso denegado)");
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

/**
 * Función para obtener las unidades de disco en Linux
 * Filtra dispositivos que no son relevantes, como dispositivos de bucle, zram o aquellos con tamaño menor a 10 MB
 * @return Vector de cadenas con las unidades de disco encontradas
 */
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

        if (!(iss >> name >> size_bytes))
            continue;
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
        entry << "/dev/" << name << " (" << format_size(size_bytes) << ")";

        if (!fstype.empty())
        {
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
