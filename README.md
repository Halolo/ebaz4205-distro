# ebaz4205-distro
Basic Yocto distribution builder for the ebaz4205 board

## hardware setup
*TBD*

## build instructions

```
git clone --recurse-submodules git@github.com:Halolo/ebaz4205-distro.git
cd ebaz4205-distro
. ebaz4205-init-build-env
bitbake ebaz4205-image
```

## flash

### SD card
* create a 100M FAT32 *boot* partition for the First Stage BootLoader and u-boot
* create an ext4 *root* partition for root file system
* form the deploy directory *build/tmp-glibc/deploy/images/ebaz4205-zynq7/*:
	* copy **u-boot.bin** in the *boot* partition
	* extract **ebaz4205-image-ebaz4205-zynq7.tar.gz** in the *root* partition
* copy your **BOOT.bin** file cretated via *Xilinx Vivado/Vitis 2020.2*. It should contain:
	* the first stage bootloader able to load and run **u-boot.bin**
	* the FPGA bitstream generated in Vivado *2020.2*
