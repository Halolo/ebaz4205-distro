SUMMARY = "Addressable leds driver"
DESCRIPTION = "Use to control led strip like ws2812b"
LICENSE = "MIT"

inherit cmake externalsrc systemd

DEPENDS_append = "paho-mqtt-c"
RDEPENDS_${PN}_append = "paho-mqtt-c"

SYSTEMD_SERVICE_${PN} = "${PN}.service"
SYSTEMD_AUTO_ENABLE_${PN} = "enable"

SRC_URI = "file://${PN}.service"

EXTERNALSRC = "${THISDIR}/src"

do_install_append() {
	install -d ${D}${systemd_system_unitdir}
	install -m 0644 ${WORKDIR}/${PN}.service ${D}${systemd_system_unitdir}
}
