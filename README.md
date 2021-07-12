# ebaz4205-distro
Basic Yocto distribution builder for the ebaz4205 board

## hardware setup
See [the vivado demo project](https://github.com/Halolo/ebaz4205-vivado).

## image
The **ebaz4205-image** bitbake target is a demo image based on **core-image-base**.

It adds a library to interface with the LED strip controllers as well as a demo app producing a rainbow effect on the ws2812b controller pin.
The demo app has a systemd service enabled by default.

## fsbl
To simplify the workflow, the image will also build the first stage bootloader: **fsbl.elf** and optionally the *bootimage*: **BOOT.bin**.
- The *FSBL* will load **u-boot.bin** from the boot partiton.
- It is not usable as-is and a *bootimage* has to be generated from it and the FPGA bitstream: **BOOT.bin**.
- Providing the path of the FPGA bitstream in the **BITSTREAM_PATH** variable will result in the creation of the *bootimage* by the **ebaz4205-image** bitbake target.
- If not provided, **BITSTREAM_PATH** defaults to */tmp/ebaz4205_wrapper.bit*, and if the file doesn't exists, the *bootimage* won't be created, just the *FSBL*.
- The *FSBL* and *bootimage* can also be built separatly of course but using the bitbake multiconfig feature:

```
bitbake mc:fsbl:zynq-fsbl
```

The reason is that the toolchain used to build it is not the same as the toolchain used to build the Linux system.

The file(s) will be available under **tmp-cortexa9-zynq-newlib/deploy/images/cortexa9-zynq/**

## build instructions

```
git clone --recurse-submodules git@github.com:Halolo/ebaz4205-distro.git
cd ebaz4205-distro
BITSTREAM_PATH=/path/to/bitstream.bit . ebaz4205-init-build-env
bitbake ebaz4205-image
```

## flash

### SD card
* create a 100M FAT32 *boot* partition for the First Stage BootLoader and u-boot
* create an ext4 *root* partition for root file system
* from the deploy directory **build/tmp-glibc/deploy/images/ebaz4205-zynq7/**:
	* copy **u-boot.bin** in the *boot* partition
	* extract **ebaz4205-image-ebaz4205-zynq7.tar.gz** in the *root* partition
* from the deploy directory **build/tmp-cortexa9-zynq-newlib/deploy/images/cortexa9-zynq/**:
	* copy **BOOT.bin** in the *boot* partition
