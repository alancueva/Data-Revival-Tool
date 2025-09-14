#include "../../include/utils/SizeFormatter.h"
#include <string>
#include <iomanip>
#include <sstream>

/**
 * Función para formatear el tamaño en una cadena legible
 * Ejemplo: 1024 bytes → "1.00 KB", 1048576 bytes → "1.00 MB"
 * Utiliza un formato de dos decimales y unidades   
 * Bytes, KB, MB, GB, TB, PB
 * @param bytes Tamaño en bytes
 * @return Cadena formateada con el tamaño y la unidad
 */
std::string format_size(uint64_t bytes)
{
    if (bytes == 0) return "0 Bytes";

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