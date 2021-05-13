SUMMARY = "Addressable leds driver"
DESCRIPTION = "Use to control led strip like ws2812b"
LICENSE = "CLOSED"

inherit cmake externalsrc systemd

SYSTEMD_SERVICE_${PN} = "${PN}.service"
SYSTEMD_AUTO_ENABLE_${PN} = "enable"

SRC_URI = "file://${PN}.service"

EXTERNALSRC = "${THISDIR}/src"

do_install_append() {
	install -d ${D}${systemd_system_unitdir}
	install -m 0644 ${WORKDIR}/${PN}.service ${D}${systemd_system_unitdir}
}
