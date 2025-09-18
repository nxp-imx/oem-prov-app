# Table of Contents

1. [Introduction](#1-introduction)
2. [Toolchain](#2-toolchain)
   - 2.1. [Install in a default path](#21-install-in-a-default-path)
   - 2.2. [Install in a custom path](#22-install-in-a-custom-path)
   - 2.3. [Additional toolchain options](#23-additional-toolchain-options)
3. [External dependencies](#3-external-dependencies)
   - 3.1. [Building Cyaml](#31-building-cyaml)
     - 3.1.1. [Building Yaml](#311-building-yaml)
     - 3.1.2. [Building Cyaml](#312-building-cyaml)
   - 3.2. [Building SMW](#32-building-smw)
   - 3.3. [Building EdgeLock 2GO Agent](#33-building-edgelock-2go-agent)
     - 3.3.1. [Building the MBedTLS library](#331-building-the-mbedtls-library)
     - 3.3.2. [Building the EdgeLock 2GO Agent](#332-building-the-edgelock-2go-agent)
4. [Project configuration and compilation](#4-project-configuration-and-compilation)
5. [Useful scripts](#5-useful-scripts)
   - 5.1. [Configure script](#51-configure-script)
   - 5.2. [Build script](#52-build-script)
   - 5.3. [Example of configuring and building the project](#53-example-of-configuring-and-building-the-project)
## 1. Introduction
This guide outlines the steps to build the __OEM Provisioning Application__, including dependencies, configuration, compilation, and installation.
The OEM Provisioning Application can be built as part of the __i.MX Linux OS BSP__ using the Yocto build system. In this setup, __Yocto__ handles the configuration, compilation, and installation processes automatically, placing the resulting binary under __/usr/bin__.

Alternatively, the application can be built __outside of Yocto__. In this case, all required dependencies must be manually built and configured before building the application.

## 2. Toolchain
The OEM Provisioning Application is designed to run on ARM-based processors, specifically targeting the 64-bit ARM architecture. As such, a 64-bit GNU ARM toolchain is required for cross-compilation. Additionally, a toolchain configuration file must be provided for use by other scripts during the compilation of sub-components, ensuring the correct toolchain settings are applied.

### 2.1. Install in a default path
To install the toolchain into the ```./toolchains``` directory within the source tree, run the following CMake script:
```sh
cmake -DFORCE_TOOLCHAIN_INSTALL=True -P ./scripts/aarch64_toolchain.cmake
```
This command will download the required toolchain from the Arm developer website and place it in the ```./toolchains``` folder at the root of the project. The toolchain will then be extracted automatically.

### 2.2. Install in a custom path
To install the toolchain in a custom directory, execute the following CMake script:
```sh
cmake -DFORCE_TOOLCHAIN_INSTALL=True -DTOOLCHAIN_PATH=[install path] -P ./scripts/aarch64_toolchain.cmake
```
This configuration will download the toolchain from the developer arm website into the given path specified with the TOOLCHAIN_PATH option.

Alternatively, you can use the provided script to achieve the same result:
```sh
 ./scripts/build.sh toolchain toolpath=[install path]
```
### 2.3. Additional toolchain options
Users can customize additional toolchain options, such as selecting a specific toolchain revision when multiple versions are available.
<table>
<caption id="table-additional-toolchain-options">Additional toolchain options</caption>
<thead>
<tr>
  <th>Option</th>
  <th>Description</th>
</tr>
</thead>
<tbody>
<tr>
  <td>TOOLCHAIN_NAME</td>
	<td>Specifies a GNU ARM toolchain name (e.g. aarch64-linux-gnu)</td>
</tr>
<tr>
  <td>TOOLCHAIN_VERSION</td>
	<td>Specifies a specific GNU ARM toolchain version (other than default one).
	This option combined with the TOOLCHAIN_NAME is used to defined the toolchain
	complete name `arm-gnu-toolchain-[TOOLCHAIN_VERSION]-x86_64-[TOOLCHAIN_NAME]`</td>
</tr>
</tbody>
</table>

## 3. External dependencies
The __OEM Provisioning Application__ requires several external libraries and their corresponding header files to build. These libraries provide essential tasks such as secure communication with the EdgeLock 2GO Server, configuration, and interaction with security subsystems.
<table>
<caption id=#table-external-dependencies> External dependencies</caption>
<thead>
<tr>
	<th>External dependency</th>
	<th>Type</th>
	<th>Comments</th>
</tr>
</thead>
<tbody>
<tr>
	<td><b>CYaml library</b></td>
	<td>Shared library</td>
	<td>Provides schema-based YAML parsing (<code>libcyaml.so</code>).
	</td>
</tr>
<tr>
	<td><b><a href="https://github.com/nxp-imx/imx-smw">Security Middleware Library (SMW)</a></b></td>
	<td>Shared library</td>
	<td>The Security Middleware Library (SMW) provides a unified API to interract with the i.MX Secure Subsystems (<code>libsmw.so</code>) </td>
</tr>
<tr>
	<td><b><a href="https://github.com/NXP/el2go-agent">EdgeLock 2GO Agent libraries</a></b></td>
	<td>Shared libraries</td>
	<td>Handles communication with EdgeLock 2GO Server. The libraries include: <code>libnxp_iot_agent.so and libnxp_iot_agent_common.so </code> </td>
</tr>
</tbody>
</table>
The table above lists the direct dependencies of the application. Due to the way CMake handles linking, the application does not directly link against all indirect dependencies—those required by the main dependencies. However, to build the OEM Provisioning Application as a standalone project, all dependencies, including indirect ones, must be compiled. The following table provides details on both direct and indirect dependencies.

<table>
<caption id="table-external-dependencies-build-options">Indirect build dependencies</caption>
<thead>
<tr>
	<th>Direct dependency</th>
	<th>Indirect dependency</th>
	<th>Type</th>
	<th>Comments</th>
</tr>
</thead>
<tbody>
<tr>
	<td >CYaml library</td>
	<td>Yaml Library</td>
	<td>Shared library</td>
	<td>Provides low level Yaml parsing (<code>libyaml.so</code></td>
</tr>
<tr>
	<td>SMW library</td>
	<td> N/A</td>
	<td> N/A</td>
	<td> The SMW (Security Middleware) library has its own set of external dependencies, which are not detailed in this documentation. For a complete list, please refer to the <a href="https://github.com/nxp-imx/imx-smw">Security Middleware Library (SMW) Documentation</a> </td>

</tr>
<tr>
	<td rowspan="2">EdgeLock 2GO Agent</td>
	<td>MbedTLS</td>
	<td>Static library</td>
	<td>The MbedTLS libraries are used by the EdgeLock 2GO Agent to establish secure connections with the EdgeLock 2GO Server.</td>
</tr>
<tr>
	<td>SMW</td>
	<td>Shared library</td>
	<td>See <a href="https://github.com/nxp-imx/imx-smw">Security Middleware Library (SMW) Documentation</a></td>
</tr>
</tbody>
</table>

### 3.1. Building Cyaml
#### 3.1.1 Building Yaml
The Yaml library is a low-level parser used by Cyaml, a schema-based YAML parsing library, to process YAML files. The OEM Provisioning Application utilizes Cyaml to parse its configuration file.
The following script can be used to build Yaml:
```sh
cmake -DTOOLCHAIN_PATH=[toolchain path] -DCMAKE_TOOLCHAIN_FILE=./scripts/aarch64_toolchain.cmake -DYAML_SRC_PATH=[source path] -DYaml_ROOT=[export path] -P ./scripts/build_yaml.cmake
```
Alternatively, you can use the provided script to achieve the same result:
```sh
./scripts/build.sh yaml toolpath=[tool path] export=[export path] src=[source path]
```
The scripts download the Yaml library into the specified source folder, build it, and then export the resulting binaries to the designated export folder.
#### 3.1.1 Building Cyaml
The Cyaml library is a schema-based YAML parsing library used by the OEM Provisioning Application to parse its configuration file.
The following script can be used to build Cyaml:
```sh
cmake -DTOOLCHAIN_PATH=[toolchain path] -DCMAKE_TOOLCHAIN_FILE=./scripts/aarch64_toolchain.cmake -DCYAML_SRC_PATH=[source path] -DCyaml_ROOT=[export path] -DCMAKE_PREFIX_PATH=[yaml export path] -P ./scripts/build_cyaml.cmake
```
Alternatively, you can use the provided script to achieve the same result:
```sh
./scripts/build.sh cyaml toolpath=[tool path] export=[export path] src=[source path]
```
>  **Note:**
> The build.sh script assumes that the yaml library is located in the export path.

The scripts download the CYaml library into the specified source folder, build it, and then export the resulting binaries to the designated export folder.
## 3.2. Building SMW
The OEM Provisioning Application uses the __Security Middleware (SMW) library__ to execute operations on the EdgeLock Secure Enclave. The SMW library has its own set of dependencies, which are not covered in this document. For detailed instructions on how to build each SMW dependency, please refer to the <a href="https://github.com/nxp-imx/imx-smw">Security Middleware Library (SMW) Documentation</a>
Alternatively, you can use the provided script to build the SMW library and its dependencies:
```sh
./scripts/build.sh smw toolpath=[tool path] out=[smw build folder path] src=[source path] platform=[imx93evk/imx91evk/imx8ulpevk] export=[smw export path]
```
The scripts assume that the SMW is available in the __[source path]__ together with the needed dependencies (see  <a href="https://github.com/nxp-imx/imx-smw">Security Middleware Library (SMW) Documentation</a>).
## 3.3. Building EdgeLock 2GO Agent
The OEM Provisioning Application uses the EdgeLock 2GO Agent to securely communicate with the EdgeLock 2GO Server and retrieve secure objects. To establish a secure connection, the EdgeLock 2GO Agent relies on the MbedTLS library. Additionally, the agent uses the SMW (Security Middleware) library to perform operations—such as importing secure objects—on the EdgeLock Secure Enclave.
### 3.3.1. Building the MBedTLS library
The following script can be used to build MBedTLS:
```sh
cmake -DTOOLCHAIN_PATH=[tool path] -DCMAKE_TOOLCHAIN_FILE=./scripts/aarch64_toolchain.cmake -DMbedTLS_BUILD_DIR=[build path] -DMbedTLS_SRC_PATH=[source path] -DMbedTLS_ROOT=[export path] -P ./scripts/build_mbedtls.cmake
```
Alternatively, you can use the provided script to achieve the same result:
```sh
./scripts/build.sh mbedtls toolpath=[tool path] export=[export path] src=[source path] out=[build path]
```
The scripts download the MBedTLS sources into the specified source folder, built it, and then export the resulting binaries to the designated export folder.

### 3.3.2. Building the EdgeLock 2GO Agent
The following script can be used to build EdgeLock 2GO Agent:
```sh
cmake -DTOOLCHAIN_PATH=[tool path] -DCMAKE_TOOLCHAIN_FILE=./scripts/aarch64_toolchain.cmake -Del2go_agent_SRC_PATH=[source path] -DNXP_SMW_DIR=[SMW cmake config dir]-Del2go_agent_ROOT=[export path] -DMbedTLS_DIR=[MbedTLS cmake config dir] -Del2go_agent_BUILD_DIR=[build path] -P ./scripts/build_el2go_agent.cmake
```
Alternatively, you can use the provided script to achieve the same result:
```sh
./scripts/build.sh el2go_agent toolpath=[tool path] export=[export path] src=[source path] smw=[SMW cmake config dir] mbedtls=[MBedTLS config dir] out=[build dir]
```
## 4. Project configuration and compilation
Before building the project it must be configured and all the dependencies resolved.

The project uses CMake as its build system, with several key configuration options:

<table>
<caption id=#table-build-environment-options> Build environment options</caption>
<thead>
<tr>
	<th>Project cmake variable</th>
	<th>CMake option</th>
	<th>Description</th>
</tr>
</thead>
<tbody>
<tr>
	<td>CMAKE_TOOLCHAIN_FILE</td>
	<td>-DCMAKE_TOOLCHAIN_FILE=[/path/to/script]</td>
	<td>Path to the script configuring the cross-compiler toolchain. The following script can be used: <code>./scripts/aarch64_toolchain.cmake</code>
	</td>
</tr>
<tr>
	<td>TOOLCHAIN_PATH</td>
	<td>-DTOOLCHAIN_PATH=[/path/to/toolchain]</td>
	<td>Configure the toolchain path if it's not the default one. Must be the path to the folder containing the toolchain folder.</code>
	</td>
</tr>
<tr>
	<td>CMAKE_BUILD_TYPE</td>
	<td>-DCMAKE_BUILD_TYPE=[Debug/Release]</td>
	<td>The build type of the project can be configured, with <b>Release</b> being the default option. </code>
	</td>
</tr>
<tr>
	<td>VERBOSE</td>
	<td>-DVERBOE=n</td>
	<td>Configures the debug verbosity:
		<ul>0 &rarr; No debug </ul>
		<ul>1 &rarr; ERROR - messages only </ul>
		<ul>2 &rarr; INFO - errors + information messages </ul>
		<ul>3 &rarr; DEBUG - all of the above + debug messages </ul>
		<ul>4 &rarr; VERBOSE - all of the above + verbose messages </ul>
	</td>
</tr>
<tr>
	<td>CMAKE_INSTALL_PREFIX</td>
	<td>-DCMAKE_INSTALL_PREFIX=[/path/to/install]</td>
	<td>Defines the installation prefix directory when executing <code>make install</code>. Default path is <code>/usr/bin</code>.
	</td>
</tr>
<tr>
	<td>NXP_SMW_DIR</td>
	<td>-DNXP_SMW_DIR=[/path/to/config]</td>
	<td>Path to the folder where configuration packages for the SMW library are located.</td>
</tr>
<tr>
	<td>el2go_agent_DIR</td>
	<td>-Del2go_agent_DIR=[/path/to/config]</td>
	<td>Path to the folder where configuration packages for the EdgeLock 2GO Agent library are located.
	</td>
</tr>
<tr>
	<td>CLOSE_COMMIT_DRY_RUN</td>
	<td>-DCLOSE_COMMIT_DRY_RUN=ON</td>
	<td> Testing purpose only. Disable irreversible operations like committing non-volatile key storage and closing the device. This option is useful for testing (e.g. interface testing) without side effects.
	</td>
</tr>
</tbody>
</table>

## 5. Useful scripts
As described in previous sections, users can choose to build project dependencies using an alternative script. Two scripts are available for this purpose: one for building dependencies and configuration and another for building specific components.

### 5.1 Configure script
```sh
./scripts/configure.sh
```
This shell script builds the project dependencies and configures the project. It assumes that the sources for external dependencies are already available (see [Dependencies table](#table-dependencies) below). The other external dependencies are downloaded by the build scripts.

Usage:
```sh
./scripts/configure.sh [build directory] [architecture] toolpath=[path/to/toolpath]
```
The ```toolpath``` parameter is optional. If not specified, the script will use the default toolchain.

<table>
<caption id="table-dependencies">Dependencies</caption>
<thead>
<tr>
	<th>Source</th>
	<th>Path</th>
	<th>Description</th>
</tr>
</thead>
<tbody>
<tr>
	<td><a href="https://github.com/nxp-imx/imx-smw">Security Middleware Library (SMW)</a></td>
	<td>../smw</td>
	<td>SMW library source. In order to build SMW, also the SMW dependencies must be available. For more details see <a href="https://github.com/nxp-imx/imx-smw">Security Middleware Library (SMW)</a></td>
</tr>
<tr>
	<td><a href="https://github.com/NXP/el2go-agent">EdgeLock 2GO Agent</a></td>
	<td>../simw-devicelink</td>
	<td>EdgeLock 2GO Agent library source</td>
</tr>
</tbody>
</table>


### 5.2 Build script
```sh
./scripts/build.sh
```

This script can be used for multiple purposes like building MBedTLS, EdgeLock 2GO Agent, and other project dependencies. It provides a flexible and standardized approach to building various components.
It can also be used to build and install the project.
For usage details, run the script with the help flag:
```sh
./scripts/build.sh --help
```

### 5.3 Example of configuring and building the project
* configure
```sh 
./scripts/configure.sh [build directory] [architecture] toolpath=[path/to/toolpath]
```
* build
```sh
./scripts/build.sh build out=[build directory]
```
* install
```sh
./scripts/build.sh install out=[build directory] dest=[install_directory]
```
>  **Note:**
> To avoid relative path issues, it is advisable to use absolute paths.

