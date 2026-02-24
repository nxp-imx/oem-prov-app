# OEM Provisioning Application - User Guide

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites for Running the OEM Provisioning Application](#prerequisites-for-running-the-oem-provisioning-application)
  - [1. Configuration File](#1-configuration-file)
  - [2. NVM Daemon](#2-nvm-daemon)
- [Running the OEM Provisioning Application](#running-the-oem-provisioning-application)
  - [Command Syntax](#command-syntax)
  - [Command-Line Options](#command-line-options)
  - [Usage Examples](#usage-examples)
  - [Option Restrictions](#option-restrictions)
- [Modes of Operation](#modes-of-operation)
  - [1. Online Provisioning (Provisioning via Cloud)](#1-online-provisioning-provisioning-via-cloud)
    - [Steps](#steps)
    - [Provisioning with Claim Code Injection](#provisioning-with-claim-code-injection)
    - [Server Certificate](#server-certificate)
  - [2. Offline Provisioning (Provisioning via Proxy)](#2-offline-provisioning-provisioning-via-proxy)
    - [Steps](#steps-1)
    - [Run Automatically at Boot Time](#run-automatically-at-boot-time)
    - [Post Provisioning Cleanup](#post-provisioning-cleanup)
    - [Mode Specific Details](#mode-specific-details)
      - [Provisioning via Proxy with Device ID](#provisioning-via-proxy-with-device-id)
      - [Provisioning via Proxy per Product Type](#provisioning-via-proxy-per-product-type)
- [Optional Post-Provisioning Actions](#optional-post-provisioning-actions)
- [Additional Features](#additional-features)
  - [Commit the Secure Storage](#commit-the-secure-storage)
  - [Forward the Device Lifecycle](#forward-the-device-lifecycle)
  - [Retrieve the Device UUID](#retrieve-the-device-uuid)
  - [Retrieve the Device Lifecycle](#retrieve-the-device-lifecycle)
  - [Claim Code Injection](#claim-code-injection)

---

# Introduction
This guide provides instructions on how to run the __OEM Provisioning Application__ in various modes of configuration. It covers __online provisioning__ and __offline provisioning__ flows, including additional features and configuration options.

# Prerequisites for Running the OEM Provisioning Application

Before executing the OEM Provisioning Application, ensure the following prerequisites are met:

## 1. Configuration File

The application uses a YAML-based configuration file to control its behavior. A template configuration file ([config.yaml](../config/config.yaml)) is provided and may be adapted to meet specific deployment requirements.

**Important notes:**
* Not all commands require a configuration file (e.g., `--uuid`, `--version`)
* Provisioning operations (`--online`, `--offline`) **require** a valid configuration file
* The configuration file must be accessible on the target system before running provisioning commands

## 2. NVM Daemon

The EdgeLock Enclave stores cryptographic keys and secure data in Non-Volatile Memory (NVM), which is managed by the ELE NVM daemon. The daemon **must be running** before executing the OEM Provisioning Application.

**Verify daemon status:**

```bash
systemctl status nvm_daemon
```

**Start the daemon if inactive:**

```bash
systemctl start nvm_daemon
```

# Running the OEM Provisioning Application

## Command Syntax

The OEM Provisioning Application is executed using the following syntax:

```bash
oem-prov-app [OPTIONS]
```

## Command-Line Options

The following table describes all available command-line options:

<table>
<caption id="table-oem-prov-options">OEM Provisioning Application Options</caption>
<thead>
<tr>
	<th>Option</th>
	<th>Description</th>
	<th>Required Argument</th>
</tr>
</thead>
<tbody>
<tr>
	<td>--online, -o config_file</td>
	<td> Runs the application in <b>the device provisioning via cloud mode</b>, connecting to EdgeLock 2GO Server to download security assets and provision the device.
	</td>
</tr>
<tr>
	<td>--offline, -f config_file</td>
	<td>Runs the application in <b>offline mode</b>, provisioning the device using locally stored assets (FAT32 partition or filesystem). Supports:
		<ul>
			<li>Provisioning via proxy with device ID (device-specific)</li>
			<li>Provisioning via proxy per product type (product-based)</li>
		</ul>
	</td>
</tr>
<tr>
	<td>--claim-code, -c	 claimcode_file</td>
	<td> Injects a <b>claim code</b> from a specified file (in base64 format) into the device.
	</td>
</tr>
<tr>
	<td>--uuid, -u</td>
	<td>Retrieves and prints the <b>device UUID</b> in hexadecimal format.</td>
</tr>
<tr>
	<td>--get-life-cycle, -g</td>
	<td>Retrieves and prints the <b>device lifecycle</b> state (open, closed, or closed-locked).</td>
</tr>
<tr>
	<td>--life-cycle, -l option</td>
	<td> Forwards the device lifecycle to <b>closed</b> or <b>closed-locked</b> making the device boot signed images only. The operation is irreversible.
	</td>
<tr>
	<td>--storage, -s commit</td>
	<td> Commits the <b>non-volatile key storage</b> to physical memory and increments the <b>hardware anti-rollback counter</b>.
	</td>
</tr>
<tr>
	<td>--verbose, -V level[0-4]</td>
	<td>Sets the verbosity level:
		<ul>0 &rarr; No debug </ul>
		<ul>1 &rarr; ERROR - messages only </ul>
		<ul>2 &rarr; INFO - errors + information messages </ul>
		<ul>3 &rarr; DEBUG - all of the above + debug messages </ul>
		<ul>4 &rarr; VERBOSE - all of the above + verbose messages </ul>
	</td>
	<td>Level: <code>0-4</code></td>
</tr>
</tbody>
</table>

## Usage Examples

**Online provisioning:**
```bash
oem-prov-app --online /etc/opt/oem-prov-app/config.yaml
```

**Offline provisioning:**
```bash
oem-prov-app --offline /etc/opt/oem-prov-app/config.yaml
```

**Retrieve device UUID:**
```bash
oem-prov-app --uuid
```

**Commit secure storage:**
```bash
oem-prov-app --storage commit
```

**Forward device lifecycle:**
```bash
oem-prov-app --lifecycle closed
```

**Enable verbose logging:**
```bash
oem-prov-app --online /etc/opt/oem-prov-app/config.yaml --verbose 3
```

## Option Restrictions

* `--online` and `--offline` are mutually exclusive
* `--storage` and `--lifecycle` must be used independently (cannot be combined with other options except `--verbose`)
* `--claim-code` can only be used with `--online` mode (not with `--offline`)
* `--verbose` can be combined with any other option

# Modes of operation
The OEM Provisioning Application supports two primary modes of operation: __online provisioning__ (__provisioning via cloud__) and __offline provisioning__ (including __device provisioning via proxy with device ID__ and __provisioning via proxy per product type__). Below, we describe each mode and provide example of usage.

## 1. Online provisioning (Provisioning via cloud)
In this mode, the application connects to EdgeLock 2GO Server via mutual TLS to download the necessary security assets and provisions them into the device.

### Steps
1. __Prepare the security assets:__
* Set up the security assets on the EdgeLock 2GO Server by creating a device group and assigning the required objects to it. Refer to the EdgeLock 2GO Server documentation for detailed instructions.
2. __Update the configuration file:__
* Modify the configuration file ([config.yaml](../config/config.yaml)) to configure the provisioning process.
3. __Run the application:__
* Execute the following command to initiate provisioning:
```sh
oem-prov-app -o /etc/opt/oem-prov-app/config.yaml
```
4. __Provisioning status:__
* The application will display a report, indicating the success or failure for each provisioning asset:

```
Update status report:
  The device update was successful (0x0001: SUCCESS)
  The correlation-id for this update is 47284cac-678c-4ff6-af61-ca6a6fe68970.
  Status for remote trust provisioning: 0x0001: SUCCESS.
    On endpoint 0x70000010, for object 0x03002000, status: 0x0001: SUCCESS.
    On endpoint 0x70000010, for object 0x20001000, status: 0x0001: SUCCESS.
    On endpoint 0x70000010, for object 0x20002001, status: 0x0001: SUCCESS.
    On endpoint 0x70000010, for object 0x30002001, status: 0x0001: SUCCESS.
```
> **Note:** The SRKH import happens only once per device. Any other attempts to import the SRKH will fail.

### Provisioning with claim code injection
It is assumed that the assets were already created and configured into the EdgeLock 2GO Server.
1. __Create and configure the claim code:__
* Create and configure the claim code in the EdgeLock 2GO Server. Consult EdgeLock 2GO Server documentation (AN12691) for detailed instructions.
* Download the claim code (even if it was provided by the user) from the EdgeLock 2GO Server and save it in a file, e.g. `claim_code.txt`.
2. __Inject claim code:__
* Use the following command to inject the claim code into the device.
```sh
oem-prov-app -c claim_code.txt
```
The local file, e.g `claim_code.txt` is deleted after the injection.

3. __Run the application in device provisioning via cloud mode:__
* Run the following command to perform the provisioning:
```sh
oem-prov-app -o /etc/opt/oem-prov-app/config.yaml
```
4. __Post-provisioning:__
* A provisioning report will be displayed as shown above
* EdgeLock 2GO Web Server interface shows that the device UUID has been successfully added to the device group

### Server certificate
By default, the EdgeLock 2GO Agent uses a built-in server certificate for TLS connection establishment. If the application targets a non-default server instance (e.g. in test or staging environments), a custom server certificate can be specified. This is configured by setting the ```server_cert``` field in the application's configuration file to the path of the desired DER-encoded certificate.

## 2. Offline provisioning (Provisioning via Proxy)

The __OEM Provisioning Application__ supports two offline provisioning modes:
* Provisioning via Proxy with device ID
* Provisioning via Proxy per product type

From the application's perspective, both modes operate similarly. The distinction lies in the nature of the security assets:
* __Provisioning via Proxy with device ID__ uses assets that are device specific, tied to the device's UUID.
* __Provisioning via Proxy per product type__ uses assets that are not UUID tied, but tied to the device family and the EdgeLock 2GO provisioning group.

The process of creating, downloading, and placing these assets on a FAT32 partition differs between the two modes. For detailed instructions, refer to EdgeLock 2GO Server documentation and  [SPSDK documentation](https://spsdk.readthedocs.io/en/latest/).

In offline provisioning, the application does not connect to the EdgeLock 2GO Server. Instead, it uses security assets stored locally, either on a partition (eMMC/SD card) or within the local filesystem, to provision the device. These assets can be written to a FAT32 partition using the [SPSDK tool](https://spsdk.readthedocs.io/en/latest/), which operates through U-Boot and therefore has access only to FAT32 partitions. However, the application also supports loading assets directly from the local filesystem, offering flexibility for testing and rapid experimentation.

### Steps
1. __Prepare Security Assets:__
* Ensure that the security assets are stored on the partition (eMMC/SD card) or local filesystem. For guidance on writing the assets refer to [SPSDK documentation](https://spsdk.readthedocs.io/en/latest/).
2. __Update the configuration file:__
* Modify the configuration file ([config.yaml](../config/config.yaml)) to configure the provisioning process.
3. __Run the application:__
* Execute the following command to provision the device in the offline mode:
```s
oem-prov-app -f /etc/opt/oem-prov-app/config.yaml
```
4. __Provisioning status:__
* The application will display a short report indicating the success or failure for each asset:
```
Object (id: 0x03002000) import: SUCCESS
Object (id: 0x30002001) import: SUCCESS
Object (id: 0x20002001) import: SUCCESS
Object (id: 0x20001000) import: SUCCESS

```
> **Note:** The SRKH import happens only once per device. Any other attempts to import the SRKH will fail.

### Run automatically at boot time
A typical provisioning scenario may involve executing the __OEM Provisioning Application__ automatically during system boot. One recommended approach is to use systemd to manage this behavior by creating a dedicated service file. We provide an example Yocto layer that sets up the required systemd service, enabling the application to run at boot and provision the security assets. Additionally, this layer configures the ```CONFIG_CONSOLE_MUX``` U-boot configuration variable, which is used by the __SPSDK__ to write the security assets on the partition. For detailed guidance, refer to the [SPSDK documentation](https://spsdk.readthedocs.io/en/latest/examples/el2go/imx93/imx93_el2go_provisioning.html).

#### Steps
1. __Setup the Yocto layer:__
* Follow the [Yocto Layer Readme](./meta-oem-prov-app/README.md) to apply the Yocto Layer
2. __Prepare Security Assets:__
* Use SPDK to load the security assets on the partition. For detailed steps refer to [SPSDK documentation](https://spsdk.readthedocs.io/en/latest/).
   * Build the i.MX Linux OS BSP after applying the Layer. For details refer to i.MX Linux OS BSP documentation.
   * Write the images on the eMMC/SD card
   * Use SPSDK to download the security assets from the EdgeLock 2GO Server
   * Use SPSDK to write the security assets on the local partition
3. __Boot the system:__
* The system is automatically booted by SPSDK, the application starts automatically after boot, running in offline mode (provisioning via proxy or product based provisioning dependending on the type of assets that were written into the FAT32 partition).
4. __Check status:__
* You can check the status of operation with:
```sh
systemctl status oem-prov
```
> **Additional Notes:**
The OEM Provisioning Application itself only performs the provisioning task - it does not reboot the system afterward. If desired, users can modify the systemd service to reboot the device upon provisioning.

### Post provisioning cleanup
After provisioning, the user may choose to __delete the assets file__ by configuring this behavior in the [config.yaml](../config/config.yaml) file.

This option is particularly useful when boot-time provisioning is enabled, but the user does not want the provisioning process to repeat on every boot. By removing the assets file after the initial provisioning, the system ensures that provisioning only occurs once.

### Mode specific details
#### Provisioning via proxy with device ID
* Assets are tied to the device UUID
* The application does not need to know the mode explicitly, but setting ```provisioning: individual``` in the config file enables stricter validation and early error detection. If ```individual``` provisioning is set in the configuration file, the application will give an error if it finds the OEM key.


#### Provisioning via Proxy per product type
* Assets are not device specific
* Assets are tied to the device family
* Assets are depending on the OEM Secret Shared Key (automatically generated by the EdgeLock 2GO Server). For more details refer to EdgeLock 2GO Server documentation.
* A special blob containing the OEM Secret Shared Key must be imported first.
* The application automatically searches for this blob and imports it if found.
* Setting ```provisioning: product``` in the configuration file enables early error reporting if the key blob is missing. The application will report an error if the OEM key is not found.
* If the key is not present, provisioning will fail regardless of the configuration because the EdgeLock Enclave cannot unpack the security objects.
* This mode may not be available for all device families and all device lifecycles. For more information refer to the EdgeLock 2GO Server documentation.

## Optional Post-Provisioning Actions
The user can configure the application (through the configuration file) to perform the following actions at the end of provisioning. These actions have __irreversible__ effects.
1. __Commit the secure storage:__
* Commits the secure key-storage to physical memory and increment the hardware anti-rollback. This action can be also executed via command line (see [Additional features](#additional-features)).

2. __Forward the device lifecycle:__
* Transitions the device to a __closed__ or __closed-locked__ state. This action can be also executed via command line (see [Additional features](#additional-features)).
> **Note:** The device lifecycle cannot transition to the __closed__ state if the SRKH was provisioned during the same boot cycle. A reboot is required after the SRKH provisioning to enable lifecycle forwarding to __closed__. Additionally, no AHAB events should be reported during boot image authentication..


## Additional features

### Commit the secure storage
The application supports committing the non-volatile key storage to physical memory and incrementing the anti-rollback counter. This can be done in one of the following ways:
* __Automatically__ after the provisioning process (if configured in the configuration file)
* __Manually__, using the command:
```sh
oem-prov-app -s commit
```
> **Note:** This command must be used exclusively. Combining it with other options is not permitted to prevent conflicts between command-line arguments and configuration file settings.

### Forward the device lifecycle
The application allows transitioning the device lifecycle to __closed__ or __closed-locked__, which ensures that the device will only boot signed images. This can be performed:
* __Automatically__ after the provisioning process (if configured in the configuration file)
* __Manually__, using this command:
```sh
oem-prov-app -l closed
```
or
```sh
oem-prov-app -l closed-locked
```
> **Note:** This command must be used exclusively. Combining it with other options is not permitted to prevent conflicts between command-line arguments and configuration file settings.

> **Note:** Forwarding the device lifecycle to __closed__ state can only be performed if the SRKH was provisioned during a previous boot cycle and no AHAB events were reported during boot image authentication.

### Retrieve the device UUID
To retrieve the device UUID, which can be useful during development, run the following command:
```sh
oem-prov-app -u
```

### Retrieve the device lifecycle
To retrieve the device lifecycle, which can be useful during development, run the following command:
```sh
oem-prov-app -g
```

### Claim code injection
Claim code injection is described in detail in [Device provisioning via cloud](#1-device-provisioning-via-cloud). The claim code option can only be used with online (Provisioning via Cloud) mode.