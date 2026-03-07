#include "HWID.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

std::string HWID::get() {

#ifdef _WIN32

    DWORD serial = 0;
    GetVolumeInformationA(
        "C:\\",
        NULL,
        0,
        &serial,
        NULL,
        NULL,
        NULL,
        0
    );

    return std::to_string(serial);

#else

    return "unknown-device";

#endif

}