# Introduction
This guide provides instructions on how to run the __OEM Provisioning Application__ in various modes of configuration. It covers both the __online__ and __indirect__ provisioning flows, including additional features and configuration options.

# Running the OEM Provisioning Application

The OEM Provisioning Application can be executed with the following command:
```sh
oem-prov-app [OPTIONS]
```

For detailed description of the available options, refer to the
<a href="#table-oem-prov-options"/>OEM Provisioning Application Options</a> table bellow.

<table>
<caption id="table-oem-prov-options">OEM Provisioning Application Options</caption>
<thead>
<tr>
	<th>Option</th>
	<th>Description</th>
</tr>
</thead>
<tbody>
<tr>
	<td>--online, -o config_file</td>
	<td> Runs the application in <b>online mode</b>, connecting to EdgeLock 2GO Server to download security assets and provision the device.
	</td>
</tr>
<tr>
	<td>--indirect, -i config_file</td>
	<td> Runs the application in <b>indirect mode</b>, provisioning the device using assets stored in a file on FAT32 partition.
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
	<td>--lifecycle, -l option</td>
	<td> Changes the device lifecycle to <b>closed</b> or <b>closed-locked</b> making the device boot signed images only. The operation is irreversible.
	</td>
<tr>
	<td>--storage, -s commit</td>
	<td> Commits the <b>non-volatile key storage</b> to physical memory and increments the <b>hardware anti-rollback counter</b>.
	</td>
</tr>
</tbody>
</table>

# Configuration file
The configuration file of the OEM Provisioning Application is managed through a customizable yaml file. The user can modify the template provided ([config.yaml](../config/config.yaml)).

# Modes of operation
The OEM Provisioning Application supports two primary modes of operation: __direct online mode__ and __indirect mode__. Bellow, we describe each mode and provide example of usage.

## 1. Direct online mode
In online mode, the application connects to EdgeLock 2GO Server via mutual TLS to download the necessary security assets and provisions them into the device.

### Steps
1. __Prepare the security assets:__
* Configure the security assets on the EdgeLock 2GO Server. Refer to the EdgeLock 2GO Server documentation for guidance.
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
### Provisioning with claim code injection
It is assumed that the assets were already created and configured into the EdgeLock 2GO Server.
1. __Create and configure the claim code:__
* Generate the claim code in the EdgeLock 2GO Server. Consult EdgeLock 2GO Server documentation (AN12691) for detailed instructions.
* Download the claim code (even if it was provided by the user) from the EdgeLock 2GO Server and save it in a file, e.g. `claim_code.txt`.
2. __Inject claim code:__
* Use the following command to inject the claim code into the device.
```sh
oem-prov-app -c claim_code.txt
```
The local file, e.g `claim_code.txt` is deleted after the injection.

3. __Run the application in online mode:__
* Run the following command to perform the provisioning:
```sh
oem-prov-app -o /etc/opt/oem-prov-app/config.yaml
```
4. __Post-provisioning:__
* A provisioning report will be displayed as shown above
* EdgeLock 2GO Web Server interface shows that the device UUID has been successfully added to the device group

### Server certificate
By default, the EdgeLock 2GO Agent uses a built-in server certificate for TLS connection establishment. If the application targets a non-default server instance (e.g. in test or staging environments), a custom server certificate can be specified. This is configured by setting the ```server_cert``` field in the application's configuration file to the path of the desired DER-encoded certificate.
## Indirect mode
In __indirect mode__, the application does not connect to the EdgeLock 2GO Server. Instead, the security assets are provided from a local partition (eMMC/SD card), and the application provisions the device using these assets.
### Steps
1. __Prepare Security Assets:__
* Ensure that the security assets are stored on the partition (eMMC/SD card). For guidance on writing the assets refer to [SPSDK documentation](https://spsdk.readthedocs.io/en/latest/).
2. __Update the configuration file:__
* Modify the configuration file ([config.yaml](../config/config.yaml)) to configure the provisioning process.
3. __Run the application:__
* Execute the following command to provision the device in indirect mode:
```s
oem-prov-app -i /etc/opt/oem-prov-app/config.yaml
```
4. __Provisioning status:__
* The application will display a short report indicating the success or failure for each asset:
```
Object (id: 0x03002000) import: SUCCESS
Object (id: 0x30002001) import: SUCCESS
Object (id: 0x20002001) import: SUCCESS
Object (id: 0x20001000) import: SUCCESS

```
### Run automatically at boot time
You can configure the OEM Provisioning Application to run at boot time by applying a __Yocto layer__. This allows the application to automatically run when the device boots, provisioning it with the security assets.
1. __Setup the Yocto layer:__
* Follow the [Yocto Layer Readme](../meta-oem-prov-app/README.md) to apply the Yocto Layer
2. __Prepare Security Assets:__
* Use SPDK to load the security assets on the partition. For detailed steps refer to [SPSDK documentation](https://spsdk.readthedocs.io/en/latest/).
   * Build the i.MX Linux OS BSP after applying the Layer. For details refer to i.MX Linux OS BSP documentation.
   * Write the images on the eMMC/SD card
   * Use SPSDK to download the security assets from the EdgeLock 2GO Server
   * Use SPSDK to write the security assets on the local partition
3. __Boot the system:__
* The system is automatically booted by SPSDK, the application will start automatically after boot, running in indirect mode.
4. __Check status:__
* You can check the status of operation with:
```sh
systemctl status oem-prov
```
### Post provisioning cleanup
After provisioning, the user may choose to __delete the assets file__ by configuring this behavior in the [config.yaml](../config/config.yaml) file.

This option is particularly useful when boot-time provisioning is enabled, but the user does not want the provisioning process to repeat on every boot. By removing the assets file after the initial provisioning, the system ensures that provisioning only occurs once.

## Optional Post-Provisioning Actions
The user can configure the application (through the configuration file) to perform the following actions at the end of provisioning. These actions have __irreversible__ effects.
1. __Commit the secure storage:__
* Commits the secure key-storage to physical memory and increment the hardware anti-rollback. This action can be also executed via command line (see [Additional features](#additional-features)).

2. __Close the device:__
* Transitions the device to a __closed__ or __closed-locked__ state. This action can be also executed via command line (see [Additional features](#additional-features)).


## Additional features

### Commit the secure storage
The application can commit the non-volatile key storage into physical memory and increment the anti-rollback counter. This can be performed:
* Automatically after provisioning (if configured in the configuration file)
* By executing:
```sh
oem-prov-app -s commit
```

### Close the device
The application allows transitioning the device lifecycle to __closed__ or __closed-locked__, which ensures that the device will only boot signed images. This can be performed:
* Automatically after provisioning (if configured in the configuration file)
* By executing:
```sh
oem-prov-app -l closed
```
or
```sh
oem-prov-app -l closed-locked
```

###

### Retrieve the device UUID
To retrieve the device UUID, which can be useful during development, run the following command:
```sh
oem-prov-app -u
```

### Claim code injection
Claim code injection is described in detail in [Direct online mode](#1-direct-online-mode)