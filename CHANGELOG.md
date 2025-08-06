# List of changes
This file briefly describes the list of changes and bug fixes for each OEM Provisioning Application release.

# List of releases
1. [Release 2.0](#release-20)
2. [Release 1.1](#release-11)
3. [Release 1.0](#release-10)

### **Release 2.0**
* Changed command line arguments for Provisioning by Proxy
* Added a list of assets files in the configuration file
* Added support for product based provisioning
* Removed the Yocto recipe as the application is included into the i.MX LinuxOS BSP
* The assets file can be located in the local filesystem

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