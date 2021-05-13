FILESEXTRAPATHS_prepend := "${THISDIR}/${MACHINE}:"

SRC_URI_append = " \
  file://zynq-ebaz4205.dts \
"

COMPATIBLE_MACHINE_ebaz4205-zynq7 = "ebaz4205-zynq7"
