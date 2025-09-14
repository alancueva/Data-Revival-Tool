#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <vector>

/**
 * Información sobre un disco
 */
struct DiskInfo
{
    // Ruta real del dispositivo (ej: \\.\\C: o /dev/sda)
    std::string path;    
    // Texto para mostrar en la UI (ej: C:\\ (500 GB))       
    std::string display_name;   
};

std::vector<DiskInfo> get_disks();

#endif