#pragma once

#include <string>
#include <vector>

class Device
{
public:
    Device(const char *path);
    ~Device();
    std::vector<uint8_t> hashPassword(const std::string &password);
    void saveKey(const std::vector<uint8_t> &key);
private:
    int fd;
    std::vector<uint8_t> getSerial();
};
