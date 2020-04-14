# opal-lullaby
This utility enables Linux to resume from S3 sleep when the system disk is locked using OPAL. It is designed to cooperate with [sedutil](https://github.com/Drive-Trust-Alliance/sedutil). Linux kernel has a built-in feature which can unlock OPAL disk when woken up from S3 by storing OPAL key in memory and applying it right after resume. But Linux has no ability to reach the key when PBA (pre-boot authorization) is used because the computer is rebooted after PBA is done. This problem is addressed by this utility. The source code demonstrates the simplest possible approach to get S3 sleep working with PBA.

## Build
Build requires *cmake*, *gcc* and *make*. Run following commands:
```
cmake src
make
```

## Usage
Derive OPAL key from the password using serial number of the device:
```
./opal-lullaby hash /dev/nvme0n1
```
Setup the system to run this command at boot or before entering S3 sleep:
```
./opal-lullaby save /dev/nvme0n1 [key]
```
Make sure this command can't be retrieved by an attacker. The device can be unlocked even when powered off with this key. Store the command on an encrypted drive if protecting against someone who has no access to the system. Store it in a file not readable by users if protecting against someone who has access to the system. Also make sure atacker is not able to list processes and their parameters.
