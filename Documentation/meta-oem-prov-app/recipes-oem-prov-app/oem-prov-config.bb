# Copyright 2025 NXP
SUMMARY = "Systemd service to run OEM Provisioning Application"

DESCRIPTION = "Copy oem-prov-app configuration file and run the application automatically at boot time"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://config.yaml"
SRC_URI += "file://oem-prov.service"

S = "${WORKDIR}/sources"
UNPACKDIR = "${S}"

inherit systemd

SYSTEMD_AUTO_ENABLE = "enable"
SYSTEMD_SERVICE:${PN} = "oem-prov.service"

do_install () {
    install -d ${D}/${sysconfdir}/opt/oem-prov-app
    install -m 0644 ${UNPACKDIR}/config.yaml ${D}/${sysconfdir}/opt/oem-prov-app

    install -d ${D}/${systemd_unitdir}/system
    install -m 0644 ${UNPACKDIR}/oem-prov.service ${D}/${systemd_unitdir}/system

}

FILES:${PN} += "\
	${sysconfdir}/opt/oem-prov-app \
	${systemd_unitdir}/system \
	"

