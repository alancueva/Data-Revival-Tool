#ifndef DEVICE_PHYSICAL_H
#define DEVICE_PHYSICAL_H

#include <string>

namespace DevicePhysical {

/**
 * Verifica si el dispositivo es un disco físico válido (no partición).
 * Compatible con Windows y Linux.
 *
 * @param device_path Ruta o identificador del dispositivo.
 * @return true si es disco físico, false si es partición o no reconocido.
 */
bool is_physical_disk(const std::string& device_path);

} 

#endif
