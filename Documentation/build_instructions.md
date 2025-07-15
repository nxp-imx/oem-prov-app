## Introduction
This guide outlines the steps to build the __OEM Provisioning Application__, including dependencies, configuration, compilation, and installation.
## External dependencies
The __OEM Provisioning Application__ requires several external libraries and their corresponding header files to build. These libraries provide essential tasks such as secure communication with the EdgeLock 2GO Server, configuration, and interaction with security subsystems.
<table>
<caption id=#table-external-dependencies> External dependencies options</caption>
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

## Project configuration and compilation
The project can be build using the Yocto infrastructure, the application being integrated into the __i.MX Linux OS BSP__.

Several CMake options are available to customize the build process.
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
	<td>CYAML_ROOT</td>
	<td>-DCYAML_ROOT=[/path/to/export]</td>
	<td>Path to the folder where Cyaml headers and library are located.
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

## Project installation

After successfully building the OEM Provisioning Application, the following artifacts are required for the application to run correctly:
* __Executable__: The output executable is named __oem-prov-app__ and by default is placed in the __/usr/bin__ directory.
* __Configuration file__: The configuration file __config.yaml__ needs to be present in the filesystem. The user can use and customize the template provided: [config.yaml](../config/config.yaml).

While the project can be installed using the `make install` command, which defaults to placing the files in the system directory `/usr` as defined by [CMAKE_INSTALL_PREFIX](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html), it is recommended to modify the installation directory when performing a cross-compilation to avoid installing files on the host filesystem.
### Steps to install the project to a specific directory
1. Navigate to the build directory
2. Run the following command to install the project to a specific location:\
`[builddir]$ make DESTDIR=[path/to/install] install`
* __DESTDIR__: This is the path where you want to install the files. The `/usr` folder will be created (if not already present) in the specified destination. By default, the executable will be installed to `[DESTDIR]/usr/bin`.
3. __Cross-compilation installation:__
* If the project was cross-compiled, the installation folder should be copied to the target system. For example, you can copy the executable from `[DESTDIR]/usr/bin` to the target system's `/usr/bin` folder.

>  **Note 1:**
> If the project is compiled within the Yocto environment, the executable is automatically placed in the `/us/bin/` directory of the target filesystem.

> **Note 2:**
> The configuration file is not automatically installed during the build process, as it requires customization by the user. To install the configuration file into the root filesystem, you may utilize the [Yocto Layer](./meta-oem-prov-app/README.md), which facilitates the installation of the configuration file within the Yocto-generated filesystem.




