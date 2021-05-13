FILESEXTRAPATHS_prepend := "${THISDIR}/${MACHINE}:"

SRC_URI_append = " \
  file://ebaz4205.cfg \
  file://0001-cadence-quadspi.c-use-do_div.patch \
"
