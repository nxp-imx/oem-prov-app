meta-oem-prov-app Yocto layer
===================

Overview of the Layer
---------------------
This layer contains the OEM Provisioning Application related Yocto recipes.

1. Recipes

* `oem-prov-app_git.bb`: This recipe is responsible for adding support to compile and deploy the OEM Provisioning Application to the Yocto BSP.
* `oem-prov-config.bb`: This recipe deploys the configuration file for the OEM Provisioning Application into the root filesystem under `/etc/opt/oem-prov-app`. It also configures the application to start automatically at boot time, running in provisioning via proxy mode. \
It is possible that the application runs early at boot time before the storage (such as an eMMC device) is mounted by the system. To avoid race conditions it is advisable to use a mount point different than the folder the system is using for mounting the volume.
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

Customization
-------------
If you do not need all the recipes, you can customize your layer by removing any recipes you don't need. For example if you do not need the `oem-prov-config.bb` recipe, you can remove it, but you still need to keep the `oem-prov-app_git.bb` since it adds the OEM Provisioning Application to the BSP build.\
The `oem-prov-app-setup.sh` script has a parameter that can skip the `oem-prov-config.bb` recipe:
```sh
../sources/meta-oem-prov-app/tools/oem-prov-app-setup.sh --no-config
```
