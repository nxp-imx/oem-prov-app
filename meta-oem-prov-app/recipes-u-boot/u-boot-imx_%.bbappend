# CONFIG_CONSOLE_MUX is needed by SPSDK to access the device via fastboot
do_compile:prepend() {
	for config in ${UBOOT_MACHINE}; do
		echo "CONFIG_CONSOLE_MUX=y" >> ${B}/${config}/.config
	done
}
