#include "../../include/utils/SizeFormatter.h"
#include <windows.h>
#include <string>
#include <iomanip>

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