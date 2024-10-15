# OEM Provisioning Application Project
This git repository contains the sources (C standard) for the OEM Provisioning Application.

## Overview
The OEM Provisioning Application is a reference application which performs the OEM provisioning. The OEM provisioning consists in importing the assets formatted by the EdgeLock 2GO server into the EdgeLock Enclave.
There are 3 modes of operation:

* direct on-line mode - the OEM Provisioning Application uses the EdgeLock 2GO Agent libraries to get the assets from the EdgeLock 2GO server.
* indirect mode - the assets are available in the external memory (written by an external application). The OEM Provisioning Application is reading the assets from the memory (or from the local filesystem) and imports them into the EdgeLock Enclave.
* batch mode - from the OEM Provisioning Application point of view, it is similar with the indirect mode. The assets for the particular device are taken from the memory (or local filesystem) and imported into EdgeLock 2GO Enclave.

Besides provisioning there are also some other features that the OEM Provisioning Application has:

* commit the secure storage
* close the device
* inject a claim code that it is available in a file
* retrieve the device UUID

## User guide
Project user guide can be found in the [User Guide](./Documentation/user_guide.md)

## List of changes
The list of changes can be found in the [Change Log](./CHANGELOG.md)

## License
All the sources are under <a href="https://opensource.org/license/BSD-3-clause/">BSD 3-Clause license</a>.
