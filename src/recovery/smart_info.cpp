#include "../../include/recovery/smart_info.h"

#ifdef _WIN32
#include <windows.h>
#include <winioctl.h>
#include <string>
#include <iostream>

bool get_smart_info_w(const std::string& device_path, int& health, int& temp) {
    health = -1;
    temp = -1;

    std::cout << "Intentando abrir dispositivo: " << device_path << std::endl;

    HANDLE hDevice = CreateFileA(device_path.c_str(),
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL,
                                 OPEN_EXISTING,
                                 0,
                                 NULL);

    if (hDevice == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        std::cerr << "Error al abrir dispositivo. Código: " << error << std::endl;
        return false;
    }

    STORAGE_PROPERTY_QUERY query = {};
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    STORAGE_DEVICE_DESCRIPTOR descriptor = {0};
    DWORD bytesReturned = 0;
    BYTE buffer[1024] = {0};

    if (!DeviceIoControl(hDevice,
                         IOCTL_STORAGE_QUERY_PROPERTY,
                         &query,
                         sizeof(query),
                         &descriptor,
                         sizeof(buffer),
                         &bytesReturned,
                         NULL)) {
        std::cerr << "Error al consultar propiedades. Código: " << GetLastError() << std::endl;
        CloseHandle(hDevice);
        return false;
    }

    if (descriptor.BusType != BusTypeAta && descriptor.BusType != BusTypeSata) {
        std::cerr << "El dispositivo no es ATA/SATA (BusType: " << descriptor.BusType << ")." << std::endl;
        CloseHandle(hDevice);
        return false;
    }

    BYTE identifyBuffer[sizeof(SENDCMDOUTPARAMS) + 512] = {0};
    SENDCMDINPARAMS identifyParams = {0};
    identifyParams.cBufferSize = 512;
    identifyParams.irDriveRegs.bCommandReg = ID_CMD;

    if (!DeviceIoControl(hDevice,
                         SMART_RCV_DRIVE_DATA,
                         &identifyParams,
                         sizeof(SENDCMDINPARAMS) - 1,
                         identifyBuffer,
                         sizeof(identifyBuffer),
                         &bytesReturned,
                         NULL)) {
        std::cerr << "Error al verificar SMART. Código: " << GetLastError() << std::endl;
        CloseHandle(hDevice);
        return false;
    }

    BYTE smartBuffer[sizeof(SENDCMDOUTPARAMS) + 512] = {0};
    SENDCMDINPARAMS inParams = {0};
    inParams.cBufferSize = 512;
    inParams.irDriveRegs.bFeaturesReg = READ_ATTRIBUTES;
    inParams.irDriveRegs.bCommandReg = SMART_CMD;

    if (!DeviceIoControl(hDevice,
                         SMART_RCV_DRIVE_DATA,
                         &inParams,
                         sizeof(SENDCMDINPARAMS) - 1,
                         smartBuffer,
                         sizeof(smartBuffer),
                         &bytesReturned,
                         NULL)) {
        std::cerr << "Error al leer SMART. Código: " << GetLastError() << std::endl;
        CloseHandle(hDevice);
        return false;
    }

    PSENDCMDOUTPARAMS outParams = (PSENDCMDOUTPARAMS)smartBuffer;
    if (!outParams->bBuffer) {
        std::cerr << "El búfer SMART está vacío." << std::endl;
        CloseHandle(hDevice);
        return false;
    }

    PSMART_ATTRIBUTES smartAttr = (PSMART_ATTRIBUTES)outParams->bBuffer;

    int tempFound = -1;
    int reallocated = 0;
    int pending = 0;
    int wearLeveling = 0;

    for (int i = 0; i < 30; ++i) {
        BYTE id = smartAttr->Attribute[i].Id;
        if (id == 0) continue;

        if (id == 194) {
            tempFound = smartAttr->Attribute[i].RawValue[0];
        } else if (id == 5) {
            reallocated = smartAttr->Attribute[i].RawValue[0] +
                         (smartAttr->Attribute[i].RawValue[1] << 8);
        } else if (id == 197) {
            pending = smartAttr->Attribute[i].RawValue[0] +
                      (smartAttr->Attribute[i].RawValue[1] << 8);
        } else if (id == 173 || id == 231) {
            wearLeveling = smartAttr->Attribute[i].Value;
        }
    }

    CloseHandle(hDevice);

    temp = (tempFound > 0) ? tempFound : -1;
    if (wearLeveling > 0) {
        health = wearLeveling;
    } else {
        if (reallocated == 0 && pending == 0) {
            health = 100;
        } else {
            health = std::max(10, 100 - (reallocated + pending * 2));
        }
    }

    std::cout << "Salud: " << health << "%" << std::endl;
    std::cout << "Temperatura: " << (temp > 0 ? std::to_string(temp) + "°C" : "No disponible") << std::endl;
    return true;
}

#elif __linux__
#include <cstdio>
#include <array>
#include <string>


bool get_smart_info_l(const std::string& device_path, int& health, int& temp) {
    std::string command = "smartctl -A " + device_path + " 2>/dev/null";
    std::array<char, 256> buffer;
    std::string output;

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return false;
    while (fgets(buffer.data(), buffer.size(), pipe)) {
        output += buffer.data();
    }
    pclose(pipe);

    if (output.empty()) return false;

    temp = -1;
    health = 100;

    size_t pos = output.find("194");
    if (pos != std::string::npos) {
        std::string line = output.substr(pos, output.find("\n", pos) - pos);
        int rawTemp;
        if (sscanf(line.c_str(), "%*d %*s %*s %*d %*d %*d %*d %d", &rawTemp) == 1) {
            temp = rawTemp;
        }
    }

    if (output.find("Reallocated_Sector_Ct") != std::string::npos ||
            output.find("Current_Pending_Sector") != std::string::npos) {
        health = 90; // bajamos salud si hay sectores problemáticos
    }

    return true;
}
#endif

bool get_smart_info(const std::string& device_path, int& health, int& temp) {
#ifdef _WIN32
    return get_smart_info_w(device_path, health, temp);
#elif __linux__
    return get_smart_info_l(device_path, health, temp);
#else
    return false;
#endif
}


