FILESEXTRAPATHS_prepend := "${THISDIR}/${MACHINE}:"

SRC_URI_append = " \
  file://0001-ebaz4205.patch \
  file://uEnv.txt \
"
