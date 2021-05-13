# Base this image on core-image-base
include recipes-core/images/core-image-base.bb

DEV_TOOLS = " \
  mosquitto \
  mosquitto-clients \
  devmem2 \
"

PYTHON = " \
  python3 \
  python3-pip \
"

SYSTEM = " \
  kernel-modules \
  openssh \
  vim \
  e2fsprogs \
  e2fsprogs-mke2fs \
  haveged \
"

IMAGE_INSTALL += " \
  ${DEV_TOOLS} \
  ${PYTHON} \
  ${SYSTEM} \
  addr-led \
"

EXTRA_IMAGE_FEATURES_append = " package-management"
