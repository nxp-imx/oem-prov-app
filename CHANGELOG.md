# List of changes
This file briefly describes the list of changes and bug fixes for each OEM Provisioning Application release.

# List of releases
1. [Release 2.3](#release-23)
2. [Release 2.2](#release-22)
3. [Release 2.1](#release-21)
4. [Release 2.0](#release-20)
5. [Release 1.1](#release-11)
6. [Release 1.0](#release-10)

### **Release 2.3**
* Enhanced documentation
* Retrieve the device lifecycle

### **Release 2.2**
* Updated the Yocto layer example to support whinlatter

### **Release 2.1**
* Added scripts to build oem-prov-app and dependencies
* Added startup banner for oem-prov-app
* Added -Wall compile flag
* Fix attributes settings for claim code injection
* Fix compilation issues when VERBOSE is not enabled

### **Release 2.0**
* Changed command line arguments for Provisioning by Proxy
* Added a list of assets files in the configuration file
* Added support for product based provisioning
* Removed the Yocto recipe as the application is included into the i.MX LinuxOS BSP
* The assets file can be located in the local filesystem
* Add the possibility to set the verbosity level from the command line

### **Release 1.1**
This release is compatible with i.MX Linux BSP 6.12.20_2.0.0.
* Server certificate can be provided at runtime
* Add option to delete the assets file after provisioning
* Fix reprovisioning data objects
* Remove manual search for EdgeLock 2GO agent libraries
* Changed the provisioning terminology
* Correct the offset calculation formula

### **Release 1.0**
This is the first release version of the project. The release is compatible with i.MX Linux BSP 6.12.3_1.0.0.
* Device provisioning via cloud
* Device provisioning via proxy
* Commit the secure key-storage
* Transition the device lifecycle to closed/closed-locked
* Retrieve the UUID
* Inject claim code