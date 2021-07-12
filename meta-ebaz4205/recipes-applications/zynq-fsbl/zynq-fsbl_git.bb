inherit deploy

LICENSE = "Proprietary"
LICFILENAME = "license.txt"
LIC_FILES_CHKSUM = "file://${S}/${LICFILENAME};md5=3a6e22aebf6516f0f74a82e1183f74f8"

FILESEXTRAPATHS_append = "${@ os.path.dirname(d.getVar('BITSTREAM_PATH')) if os.path.exists(d.getVar('BITSTREAM_PATH')) else ""}:"

SRCREV = "${AUTOREV}"
PV = "${XILINX_RELEASE_VERSION}+git${SRCPV}"
SRC_URI = " \
    git://github.com/xilinx/embeddedsw.git;branch=release-2020.2 \
    file://0001-Add-ebaz4205.patch \
"

BITSTREAM_FILE = "${@ os.path.basename(d.getVar('BITSTREAM_PATH'))}"
SRC_URI_append = "${@ "file://${BITSTREAM_FILE}" if os.path.exists(d.getVar('BITSTREAM_PATH')) else ""}"

COMPATIBLE_MACHINE = "${MACHINE}"
COMPATIBLE_HOST_arm = "arm-xilinx-eabi"

DEPENDS_append = "bootgen-native"

S = "${WORKDIR}/git"
B = "${S}/lib/sw_apps/zynq_fsbl/src"

PARALLEL_MAKE = ""

do_compile_prepend() {
    rm -f ${B}/ebaz4205.bif
    rm -f ${B}/BOOT.bin
    oe_runmake clean
}

do_compile_append() {
    if [ -f ${WORKDIR}/${BITSTREAM_FILE} ]; then
        # Create .bif file
    echo "\
the_ROM_image:
{
	[bootloader]${B}/fsbl.elf
	${WORKDIR}/${BITSTREAM_FILE}
}" > ${B}/ebaz4205.bif

        # generate bootimage
        bootgen -image ${B}/ebaz4205.bif -arch zynq -o ${B}/BOOT.bin
    fi
}

do_deploy() {
    install -d ${DEPLOYDIR}
    install -m 0644 ${B}/fsbl.elf ${DEPLOYDIR}/

    if [ -f ${B}/BOOT.bin ]; then
        install -m 0644 ${B}/BOOT.bin ${DEPLOYDIR}/
    fi
}

addtask deploy before do_build after do_package

FILES_${PN} = "/boot/BOOT.bin"
