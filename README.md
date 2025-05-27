# OEM Provisioning Application Project
This git repository contains the sources (C standard) for the OEM Provisioning Application.

## Overview

The OEM Provisioning Application is a tool designed to facilitate the OEM provisioning process. It supports the import of security assets into the EdgeLock Enclave. The application can operate in two main modes.
## Modes of operation
The OEM Provisioning Application supports two modes of operation:
### __Device provisioning via cloud mode__
* In this mode, the application connects directly to EdgeLock 2GO Server using the EdgeLock 2GO Agent libraries and retrieves the necessary security assets over a mutual TLS connection.
* After receiving each asset, the application provisions them into the EdgeLock Enclave.
* Optionally, the non-volatile key storage can be committed to the physical memory and device lifecycle moved to closed/closed-locked state.
### __Device Provisioning via proxy mode__
* In this mode, the security assets are already available in an external memory location, such as an eMMC/SD card on a FAT32 partition.
* The OEM Provisioning Application reads the assets from the partition and imports them into the EdgeLock Enclave.
* Optionally, the non-volatile key storage can be committed to the physical memory and device lifecycle moved to closed/closed-locked state.

## Additional features
1. __Commit the secure storage__
* The application can commit the non-volatile key storage into physical memory.
* The hardware anti-rollback counter is also incremented during this process, ensuring that the device's state cannot be rolled back to a previous insecure state.
2. __Close the device__
* The device lifecycle can be moved to closed or closed-locked states. Once closed, the device can only boot signed images.
3. __Claim Code Injection__
* The application supports the injection of a claim code into the EdgeLock Enclave.
* The claim code can be read from a file on the local file system.
4. __Retriece Device UUID__
* The application can retrieve the device UUID.

## Installation guide
Project installation guide can be found in the [Installation Guide](./Documentation/build_instructions.md)
## User guide
Project user guide can be found in the [User Guide](./Documentation/user_guide.md)

## Yocto layer
A yocto layer used to build/configure OEM Provisioning Application can be found in [Yocto layer](./meta-oem-prov-app/README.md).

## List of changes
The list of changes can be found in the [Change Log](./CHANGELOG.md)

## License
All the sources are under <a href="https://opensource.org/license/BSD-3-clause/">BSD 3-Clause license</a>.
