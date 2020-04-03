#include "device.h"

#include <algorithm>
#include <fcntl.h>
#include <linux/nvme_ioctl.h>
#include <linux/sed-opal.h>
#include <openssl/evp.h>
#include <stdexcept>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define  NVME_IDENTIFY 0x06

Device::Device(const char *path)
{
    fd = open(path, O_RDWR);
    if (fd < 0) {
        throw std::runtime_error("open failed, check permissions");
    }
}

Device::~Device()
{
    close(fd);
}

std::vector<uint8_t> Device::hashPassword(const std::string &password)
{
    std::vector<char> pass(password.begin(), password.end());
    uint8_t buff[32];
    if(PKCS5_PBKDF2_HMAC_SHA1(pass.data(), pass.size(), this->getSerial().data(), 20, 75000, 32, buff) != 1) {
        throw std::runtime_error("PKCS5_PBKDF2_HMAC_SHA1 failed");
    }
    std::vector<uint8_t> hash(buff, buff + 32);
    return hash;
}

void Device::saveKey(const std::vector<uint8_t> &key)
{
    opal_lock_unlock opal_ioctl_data = {};
    opal_ioctl_data.l_state = OPAL_RW;
    opal_ioctl_data.session.opal_key.lr = 0;
    opal_ioctl_data.session.who = OPAL_ADMIN1;
    size_t key_len = std::min(key.size(), sizeof(opal_ioctl_data.session.opal_key.key));
    memcpy(opal_ioctl_data.session.opal_key.key, key.data(), key_len);
    opal_ioctl_data.session.opal_key.key_len = key_len;
    if(ioctl(fd, IOC_OPAL_SAVE, &opal_ioctl_data)) {
        throw std::runtime_error("ioctl IOC_OPAL_SAVE failed");
    }
}

std::vector<uint8_t> Device::getSerial()
{
    struct nvme_admin_cmd cmd;
    uint8_t ctrl[4096];
    memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = NVME_IDENTIFY;
	cmd.nsid = 0;
	cmd.addr = (unsigned long)&ctrl;
	cmd.data_len = 4096;
	cmd.cdw10 = 1;
	if(ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd)) {
        throw std::runtime_error("ioctl NVME_IOCTL_ADMIN_CMD failed");
    }
    std::vector<uint8_t> serial(ctrl + 4, ctrl + 24);
    return serial;
}
