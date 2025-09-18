# OEM Provisioning Application Project
This git repository contains the sources (C standard) for the OEM Provisioning Application.

## Overview

The OEM Provisioning Application is a tool designed to facilitate the OEM provisioning process. It supports the import of security assets into the EdgeLock Enclave. The application can operate in two main modes.
## Modes of operation
The OEM Provisioning Application operates in two primary modes:
### __Device provisioning via cloud mode (online mode)__
* The application connects to the EdgeLock 2GO Server using EdgeLock 2GO Agent libraries over a mutual TLS connection.
* It retrieves and provisions security assets into the EdgeLock Enclave.
* Optionally, it can:
	* Commit non-volatile key storage to physical memory.
	* Transition the device lifecycle to closed or closed-locked state.
### __Offline modes__
There are two types of offline provisioning:

### a. Provisioning via Proxy
* Security assets are device-specific (tied to the device UUID).
* Assets are stored externally (e.g., on an eMMC/SD card on a FAT32 partition or on the local filesystem).
* The application reads and imports these assets into the EdgeLock Enclave.
### b. Product-Based Provisioning
* Security assets are not tied to a specific device UUID, but they are tied to the device family and the EdgeLock 2GO provisioning group.
* Like proxy mode, assets are read from external storage and provisioned into the enclave.
## Additional features
1. __Commit the secure storage__
* The application can commit the non-volatile key storage into physical memory.
* The hardware anti-rollback counter is also incremented during this process, ensuring that the device's state cannot be rolled back to a previous insecure state.
2. __Forward the device lifecycle__
* The device lifecycle can be moved to closed or closed-locked states. Once closed, the device can only boot signed images.
3. __Claim Code Injection__
* The application supports the injection of a claim code into the EdgeLock Enclave.
* The claim code can be read from a file on the local file system.
4. __Retrieve Device UUID__
* The application can retrieve the device UUID.

## Installation guide
Project build and installation guide can be found in the [Build and installation Guide](./Documentation/build_instructions.md)
## User guide
Project user guide can be found in the [User Guide](./Documentation/user_guide.md)

## List of changes
The list of changes can be found in the [Change Log](./CHANGELOG.md)

## License
All the sources are under <a href="https://opensource.org/license/BSD-3-clause/">BSD 3-Clause license</a>.
