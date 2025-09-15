#ifndef SMART_INFO_H
#define SMART_INFO_H

#ifdef _WIN32
#include <windows.h>
#include <winioctl.h>
#include <string>
#include <iostream>

#define ID_CMD        0xEC  // IDENTIFY DEVICE
#define SMART_CMD     0xB0
#define READ_ATTRIBUTES 0xD0

#pragma pack(push, 1)
typedef struct _SMART_ATTRIBUTE {
    BYTE  Id;
    WORD  StatusFlags;
    BYTE  Value;
    BYTE  Worst;
    BYTE  RawValue[6];
    BYTE  Reserved;
} SMART_ATTRIBUTE, *PSMART_ATTRIBUTE;

typedef struct _SMART_ATTRIBUTES {
    SMART_ATTRIBUTE Attribute[30];
    BYTE            Reserved[2];
} SMART_ATTRIBUTES, *PSMART_ATTRIBUTES;
#pragma pack(pop)



#endif

/**
 * Obtiene información SMART del disco
 * @param device_path Ruta del dispositivo
 * @param health Referencia donde se almacenará el porcentaje de salud (0-100)
 * @param temp Referencia donde se almacenará la temperatura en grados Celsius
 * @return true si se pudo obtener la información, false en caso contrario
 */
bool get_smart_info(const std::string& device_path, int& health, int& temp);

#endif