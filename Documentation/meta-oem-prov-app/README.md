meta-oem-prov-app Yocto layer
===================

Overview of the Layer
---------------------
This layer contains an example of running the OEM Provisioning Application at boot time.

1. Recipes

* `oem-prov-config.bb`: This recipe installs the configuration file for the OEM Provisioning Application into the root filesystem at `/etc/opt/oem-prov-app`. It also configures the application to automatically start at boot, running in provisioning via proxy mode.
> **Note:** The application may start early in the boot sequence, pottentially before the system mounts the storage device (e.g. eMMC). To avoid race conditions it is recommended to use a mount point that is not used by the system to mount the volume.
* `u-boot bbappend`: A bbappend file is added to modify the U-Boot configuration (set `CONFIG_CONSOLE_MUX`) which is needed by the SPSDK tool, which handles loading the security assets on the storage before the system boots. The SPSDK is accessing the device via fastboot.
2. Configuration file
* The configuration file `config.yaml` found in `meta-oem-prov-app/recipes-oem-prov-app/oem-prov-config/config.yaml` can be modified to adjust settings specific to the user scenario. If the user already has a configuration file, it should be placed in the above mentioned location.

Steps to apply the layer
-----
1. __Update/overwrite the OEM Provisioning Application configuration file__:
* Modify the [config.yaml](./recipes-oem-prov-app/oem-prov-config/config.yaml) as needed to adjust the configuration of the OEM Provisioning Application. If you already have a configuration file, just overwrite the default one from `meta-oem-prov-app/recipes-oem-prov-app/oem-prov-config` folder.
2. __Setup the layer__
* Copy the layer into the BSP `sources` folder ensuring that the Yocto build system knows about it.
3. __Run the setup script__
* The `tools/oem-prov-app-setup.sh` script will add the layer to the `conf/bblayers.conf` and configure the build system by updating the `conf/local.conf` file.
4. __Build the image__: After setting up the layer, you can build the Yocto image as usual using the bitbake tool.

Applying the layer in detail
-----------------------------

1. __Copy the layer into your BSP sources folder__:

```sh
 cp -r /path/to/meta-oem-prov-app $YOCTO_BSP/sources
 ```

2. __Update the build configuration files__:
* Add the layer to Yocto build system
```sh
bitbake-layers add-layer ../sources/meta-oem-prov-app
```
* Modify `conf/local.conf` to add the relevant configuration for the OEM Provisioning Application.
* For convenience a script was created to automatically updated the configuration files, replacing the above mentioned step 2:
```sh
../sources/meta-oem-prov-app/tools/oem-prov-app-setup.sh
```

