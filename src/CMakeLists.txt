cmake_minimum_required(VERSION 3.0)
project (opal-lullaby)
link_libraries(-lcrypto)
add_executable(opal-lullaby main.cpp device.cpp)
