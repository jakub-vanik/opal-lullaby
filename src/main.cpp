#include "device.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <vector>

struct Command
{
  std::string name;
  int (*function)(int, char **);
};

int test(int argc, char *argv[]);
int hash(int argc, char *argv[]);
int save(int argc, char *argv[]);

std::vector<Command> commands = {
    {"test", test},
    {"hash", hash},
    {"save", save}};

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    std::cout << "This program helps Linux kernel to be able to unlock OPAL device after S3 sleep." << std::endl;
    std::cout << "It is done by deriving key in same way as sedutil does and storing it into kernel." << std::endl;
    std::cout << "Kernel uses the key to unlock block device when the computer is woken up." << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << argv[0] << " test <device>" << std::endl;
    std::cout << "asks for OPAL password, derives a key and stores it into kernel." << std::endl;
    std::cout << argv[0] << " hash <device>" << std::endl;
    std::cout << "asks for OPAL password, derives a key and prints command to be executed to store the key into kernel." << std::endl;
    std::cout << argv[0] << " save <device> <key>" << std::endl;
    std::cout << "stores the key from command line into kernel." << std::endl;
    return 0;
  }
  if (argc < 2)
  {
    std::cerr << "Wrong argument count" << std::endl;
    return 1;
  }
  std::string name(argv[1]);
  for (auto it = commands.begin(); it != commands.end(); it++)
  {
    Command &command = *it;
    if (name.compare(command.name) == 0)
    {
      return command.function(argc, argv);
    }
  }
  std::cerr << "Unknown command" << std::endl;
  return 1;
}

int test(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cerr << "Wrong argument count" << std::endl;
    return 1;
  }
  try
  {
    Device device(argv[2]);
    char *pass = getpass("OPAL password: ");
    std::string password(pass, strnlen(pass, _SC_PASS_MAX));
    device.saveKey(device.hashPassword(password));
    std::cout << "OPAL key have been derived and saved to kernel." << std::endl;
    std::cout << "Try to put the computer to S3 sleep and resume now." << std::endl;
    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}

int hash(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cerr << "Wrong argument count" << std::endl;
    return 1;
  }
  try
  {
    Device device(argv[2]);
    char *pass = getpass("OPAL password: ");
    std::string password(pass, strnlen(pass, _SC_PASS_MAX));
    auto key = device.hashPassword(password);
    std::stringstream stream;
    for (auto it = key.begin(); it != key.end(); it++)
    {
      auto val = *it;
      stream << std::setfill('0') << std::setw(2) << std::hex << (int)val;
    }
    std::cout << "OPAL key have been derived. Run following command after each boot to save the key to kernel." << std::endl;
    std::cout << "Make sure this command is stored on the OPAL device itself to be protected when the device is locked." << std::endl;
    std::cout << argv[0] << " save " << stream.str() << std::endl;
    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}

int save(int argc, char *argv[])
{
  if (argc != 4)
  {
    std::cerr << "Wrong argument count" << std::endl;
    return 1;
  }
  try
  {
    Device device(argv[2]);
    std::string input(argv[3]);
    std::vector<uint8_t> key;
    std::stringstream stream;
    for (int i = 0; i < 32; i++)
    {
      stream.clear();
      stream << std::hex << input.substr(i * 2, 2);
      int val;
      stream >> val;
      key.push_back(val);
    }
    device.saveKey(key);
    std::cout << "OPAL key have been saved to kernel." << std::endl;
    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
