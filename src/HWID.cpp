#include <Windows.h>
#include <string>

std::string getHWID() {
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
}
