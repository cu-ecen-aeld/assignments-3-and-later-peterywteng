#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
        # (1.a.i) If not specified, your script should use 
        # /tmp/aeld as outdir 
	echo "Using default directory ${OUTDIR} for output"
else
        # (1.a) Take a single argument outdir which is the location 
        # on the filesystem where the output files should be placed. 
        # Replace all references to “outdir” in the remainder 
        # of the assignment with the absolute path to this directory.
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

# (1.b) Create a directory outdir if it doesn’t exist.  
# Fail if the directory could not be created. 
mkdir -p ${OUTDIR}
if [ ! -d ${OUTDIR} ]; then
        exit 1
fi

# (1.c) Build a kernel image using instructions in Module 2 lecture series.
# Note that you may need a few additional packages on your build host
#   to complete the linux build.  
# For the full and latest list of packages needed you may refer to 
#   the Dockerfile used to build the automated test container in 
#   https://github.com/cu-ecen-aeld/aesd-autotest-docker/blob/master/docker/Dockerfile
# (1.c.i) Use git to clone the linux kernel source tree 
#   if it doesn’t exist in outdir.  
# Checkout the tag specified in the manual-linux.sh script
# (1.c.i.1) Use https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
#   for the linux kernel source directory. 
# (1.c.i.1.a) Use the `--depth 1` command line argument with git 
#   if you’d like to minimize download time
# (1.d) Copy resulting files generated in step 1.c to outdir.  
cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi



# (1.e) Your script should build a root filesystem in outdir/rootfs 
#   as described in the Module 2 content, performing all of 
#   these operations in an automated fashion and completing the TODO items 
#   in the “finder-app/manual-linux.sh” script.  
# The end result should be a outdir/Image kernel image and 
#   outdir/initramfs.cpio.gz file based on the content 
#   of a staging directory tree. 
# (1.e.i) Skip the modules_install step discussed in the video content.
# The modules generated with the default kernel build are too large to fit
#   in the initramfs with default memory. 
# Alternatively you can increase the -m  argument in the start qemu scripts
#   to a value large enough to fit (currently must be >512m).  
# You don't need modules for the simple example we use in assignment 3.
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
    make -j4 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all
    #make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} modules
    #make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} dtbs 
fi

echo "Adding the Image in outdir"
cp ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
mkdir rootfs
cd rootfs
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
else
    cd busybox
fi

# TODO: Make and install busybox
make distclean
make defconfig
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

echo "Library dependencies"
#${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
#${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"
${CROSS_COMPILE}readelf -a ${OUTDIR}/rootfs/bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a ${OUTDIR}/rootfs/bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
cp ~/arm-cross-compiler/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib/ld-linux-aarch64.so.1 ${OUTDIR}/rootfs/lib
cp ~/arm-cross-compiler/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/libm.so.6 ${OUTDIR}/rootfs/lib64
cp ~/arm-cross-compiler/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/libresolv.so.2 ${OUTDIR}/rootfs/lib64
cp ~/arm-cross-compiler/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/libc.so.6 ${OUTDIR}/rootfs/lib64

# TODO: Make device nodes
cd ${OUTDIR}/rootfs
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 666 dev/console c 5 1

# TODO: Clean and build the writer utility
cd ${FINDER_APP_DIR} 
make clean
make CROSS_COMPILE=${CROSS_COMPILE}
#file writer

# (1.e.ii) Your writer application from Assignment 2 should be 
#   cross compiled and placed in the outdir/rootfs/home directory 
#   for execution on target. 
# (1.f) Copy your finder.sh, conf/username.txt, conf/assignment.txt 
#   and finder-test.sh scripts from Assignment 2 into the 
#   outdir/rootfs/home directory.
# (1.f.i) Modify the finder-test.sh script to reference 
#   conf/assignment.txt instead of ../conf/assignment.txt.
# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp writer ${OUTDIR}/rootfs/home
cp finder.sh ${OUTDIR}/rootfs/home
cp finder-test.sh ${OUTDIR}/rootfs/home
mkdir -p ${OUTDIR}/rootfs/home/conf
cp conf/username.txt ${OUTDIR}/rootfs/home/conf
cp conf/assignment.txt ${OUTDIR}/rootfs/home/conf

# (1.g) Copy the autorun-qemu.sh script into the 
#   outdir/rootfs/home directory
cp autorun-qemu.sh ${OUTDIR}/rootfs/home

# TODO: Chown the root directory
cd ${OUTDIR}/rootfs
sudo chown -R root:root *

# (1.h) Create a standalone initramfs and outdir/initramfs.cpio.gz file 
#   based on the contents of the staging directory tree. 
# TODO: Create initramfs.cpio.gz
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
cd ${OUTDIR}
gzip -f initramfs.cpio



