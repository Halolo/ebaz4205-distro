FILESEXTRAPATHS_prepend := "${THISDIR}/${MACHINE}:"

SRC_URI_append = " \
  file://eth0.network \
"

do_install_append() {
  install -m 0644 ${WORKDIR}/eth0.network ${D}${sysconfdir}/${PN}/network/
}
