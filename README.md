# OEM Provisioning Application

This repository contains the C implementation of the OEM Provisioning Application for NXP EdgeLock devices.

## Overview

The OEM Provisioning Application facilitates secure provisioning of secure assets into the EdgeLock Enclave. It supports both cloud-based and offline provisioning workflows, along with device lifecycle management and secure storage operations.

## Modes of Operation

### Device provisioning via cloud (online mode)

In online mode, the application:
* Establishes a secure mutual TLS connection to the EdgeLock 2GO Server
* Retrieves device-specific security assets from the cloud
* Provisions assets into the EdgeLock Enclave

**Optional post-provisioning operations:**
* Commit non-volatile key storage to physical memory
* Transition device lifecycle to `closed` or `closed-locked` state

### Device Provisioning via Proxy (offline modes)

The application supports two offline provisioning methods:

#### Provisioning via Proxy with device ID
* Security assets are **device-specific** (bound to individual device UUID)
* Assets are pre-generated and stored on external media (eMMC/SD card with FAT32 partition, or local filesystem)
* Application reads and imports device-specific assets into the EdgeLock Enclave

#### Provisioning via Proxy per product type
* Security assets are **product-specific** (tied to device family and EdgeLock 2GO provisioning group)
* Assets are pre-generated and stored on external media (eMMC/SD card with FAT32 partition, or local filesystem)

## Additional Features

### Secure Storage Commitment
* Commits non-volatile key storage to physical memory
* Increments hardware anti-rollback counter to prevent rollback attacks
* Ensures device state cannot be reverted to a previous insecure configuration

### Device Lifecycle Management
* Transitions device lifecycle to `closed` or `closed-locked` states
* Once closed, device can only boot cryptographically signed images

### Claim Code Injection
* Injects device claim codes into the EdgeLock Enclave
* Reads claim code from a file on the local filesystem
* Enables device registration into the EdgeLock 2GO Server

### Retrieve Device UUID
* Retrieves and displays the unique device UUID

### Retrieve Device Lifecycle
* Retrieves and displays the current device lifecycle state.

## Documentation

* **[Build and Installation Guide](./Documentation/build_instructions.md)** - Compilation and deployment instructions
* **[User Guide](./Documentation/user_guide.md)** - Usage examples and command reference
* **[Change Log](./CHANGELOG.md)** - Version history and release notes

## License

This project is licensed under the [BSD 3-Clause License](https://opensource.org/license/BSD-3-clause/).

---