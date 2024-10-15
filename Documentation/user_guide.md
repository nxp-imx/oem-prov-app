# Introduction
This guide explains how to build and run the OEM Provisioning Application.

# Toolchain
The OEM Provisioning Application runs on ARM architecture processors. To cross compile the application the necessary toolchain has to be installed. The application has been validated using the aarch[XX]-none-linux-gnu 10.3-2021.07 ([XX] can be 32 or 64 according to the target platform (32 or 64 bits)) toolchain.

# External dependencies
To build and run the OEM Provisioning Application additional headers and libraries are needed. The external dependencies must be build before building the OEM Provisioning Application.

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
	<td>Yaml library</td>
	<td>shared library</td>
	<td>A C library for parsing and emmiting YAML: libyaml.so</td>
</tr>
<tr>
	<td>CYaml library</td>
	<td>shared library</td>
	<td>Schema-based YAML parsing: libcyaml.so
		<div class="alert">
			<strong>Note</strong> The dependency on this library will be removed in the future
		</div>
	</td>
</tr>
<tr>
	<td>Security Middleware Library </td>
	<td>shared library</td>
	<td>The Security Middleware Library(SMW) allows applications to interface with i.MX Secure Subsystems through a unified API: libsmw.so </td>
</tr>
<tr>
	<td>EdgeLock 2GO Agent libraries</td>
	<td>static libraries</td>
	<td>The EdgeLock 2GO Agent libraries handles the communication with EdgeLock 2GO server and downloading and dispatching the messages from the EdgeLock 2GO Server. There are two static libraries: libnxp_iot_agent_common.a and libnxp_iot_agent.a</td>
</tr>
<tr>
	<td>MbedTLS</td>
	<td>static library</td>
	<td>Mbed TLS is a C library that implements cryptographic primitives, X.509 certificate manipulation and the SSL/TLS and DTLS protocols. 
	<div class="alert">
			<strong>Note</strong> The library used by the OEM Provisioning tool is a static library, with some customizations.
		</div>
	</td>
</tr>
</tbody>
</table>

# Project compilation

The project requires the cmake minimal version 3.13. It does not compile the dependencies, it is assumed that they are already compiled and the libraries and headers stored in locations that are passed to the build system.

The following build environment options are available:

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
	<td>Configure the debug verbosity:
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
	<td>Define the cmake project install prefix directory when executing make install. Default path is /usr/bin.
	</td>
</tr>
<tr>
	<td>NXP_SMW_DIR</td>
	<td>-DNXP_SMW_DIR=[/path/to/config]</td>
	<td>Path to the folder where to search for config-file packages for the SMW library.</td>
</tr>
<tr>
	<td>MbedTLS_DIR</td>
	<td>-DDMbedTLS_DIR=[/path/to/config]</td>
	<td>Path to the folder where to search for config-file packages for the MBedTLS library.</td>
</tr>
<tr>
	<td>EL2GOAGENT_ROOT</td>
	<td>-DDEL2GOAGENT_ROOT=[/path/to/export]</td>
	<td>Path to the folder where EdgeLock 2GO Agent headers and libraries are located.
	</td>
</tr>
<tr>
	<td>YAML_ROOT</td>
	<td>-DYAML_ROOT=[/path/to/export]</td>
	<td>Path to the folder where Yaml headers and library are located.
	</td>
</tr>
<tr>
	<td>CYAML_ROOT</td>
	<td>-DCYAML_ROOT=[/path/to/export]</td>
	<td>Path to the folder where Cyaml headers and library are located.
	</td>
</tr>
<tr>
	<td>CMAKE_INSTALL_PREFIX</td>
	<td>-DCMAKE_INSTALL_PREFIX=[/path/to/install]</td>
	<td>Define the cmake project install prefix directory when executing <em>make install</em>. Default value is /usr/local.
	</td>
</tr>
</tbody>
</table>

# Project installation

The output executable is located in the _`bin`_ sub-directory of the project build folder.

The executable can be installed using _`make install`_ command which, by default, installs the executable in the _`/usr`_ system folder.

The executable can be installed in a custom location using the following command:
`make DESTDIR=[path/to/install] install`

# Running the application

The OEM Provisioning Application can be run with the command: `oem-prov-app [OPTIONS]`.

The possible options are presented in the <a href="#table-oem-prov-options">OEM Provisioning Application options</a> table.

<table>
<caption id="table-oem-prov-options">OEM Provisioning Application options</caption>
<thead>
<tr>
	<th>Option</th>
	<th>Description</th>
	<th>Comments</th>
</tr>
</thead>
<tbody>
<tr>
	<td>--online, -o config_file</td>
	<td>The OEM Provisioning application runs in the online mode. In this mode, it connects to the EdgeLock 2GO Server and downloads the security assets which are provisioned in the device.</td>
	<td></td>
</tr>
<tr>
	<td>--indirect, -i config_file</td>
	<td>The OEM Provisioning application runs in the indirect mode. In this mode, it downloads the security assets from the external memory (or it takes them from a file) and provision the device.</td>
	<td>Not supported yet</td>
</tr>
<tr>
	<td>--claim-code, -C claimcode_file</td>
	<td>The OEM Provisioning Application takes the claim code from the claimcode_file in base64 format and injects it into the device.</td>
	<td></td>
</tr>
<tr>
	<td>--uuid, -u</td>
	<td>The OEM Provisioning Application gets the device UUID and prints it in hex format.</td>
	<td></td>
</tr>
<tr>
	<td>--close, -c</td>
	<td>The OEM Provisioning Application changes the device cycle to CLOSED</td>
	<td>Not implemented yet</td>
</tr>
</tbody>
</table>

# Configuration file
The OEM Configuration Provisioning Application uses a configuration file in yaml format:

```
#online flow configuration
online:
#The hostname used for the connection with EdgeLock 2GO server.
#It can be obtained from the GUI interface of the EdgeLock 2GO server:
#Admin Settings->Services
  hostname: "https://your-instance.edgelock2go.com"
#The port used for the communication with EdgeLock 2GO server. The
#current value to be used is 443
  port: "EdgeLock 2GO port"
#Indicates if the device will be closed after provisioning.
#Default is no. The device can be closed later using a command
#line argument
  close: 0/1
#indirect flow configuration
indirect:
#File containing the security assets to be provisioned
  json_file: "Json file containing the security assets"
  ```
