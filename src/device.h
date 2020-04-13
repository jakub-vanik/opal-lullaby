#pragma once

#include <linux/sed-opal.h>
#include <string>
#include <vector>

class Device
{
public:
  Device(const char *path);
  ~Device();
  std::vector<uint8_t> hashPassword(const std::string &password);
  bool testKey(const std::vector<uint8_t> &key);
  void saveKey(const std::vector<uint8_t> &key);

private:
  int fd;
  std::vector<uint8_t> getSerial();
  void setupIoctlData(opal_lock_unlock &opal_ioctl_data, const std::vector<uint8_t> &key);
};
