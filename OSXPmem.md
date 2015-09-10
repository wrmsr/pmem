# OSXPMem - Mac OS X Physical Memory acquisition tool #

Version: RC1

Author: Johannes Stuettgen (johannes.stuettgen@gmail.com)

## Summary ##

The OSX Memory Imager is an open source tool to acquire physical memory on an Intel based Mac. It consists of 2 components:

  * The usermode acquisition tool 'osxpmem', which parses the accessible sections of physical memory and writes them to disk in a specific format.
  * A generic kernel extension 'pmem.kext', that provides read only access to physical memory. After loading it into the kernel it provides a device file ('/dev/pmem/'), from which physical memory can be read.

## Usage ##

  1. You need root access for this to work so first open a root shell ('sudo su').
  1. Now unpack the archive ('tar xvf OSXPMem.tar.gz'). This creates a new directory 'OSXPMem' containing the binary 'osxpmem', as well as the kernel extension 'pmem.kext'.
  1. Enter the directory you just created ('cd OSXPMem').
  1. Run the imager by passing it a file-name for the memory image. ('./osxpmem memory.dump' will create a file named 'memory.dump').

The imager supports multiple output formats, at the moment these are Mach-O, ELF and zero-padded RAW. You can select which output format to use by passing the '--format' option. For example to write a Mach-O image you would invoke './osxpmem --format mach memory.dump'. The default output format is ELF.

For more information on different command line switches run './osxpmem --help'.

## Common Pitfalls ##
  1. Mac OS X only allows kernel extension to load if they are owned by the user 'root' and the group 'wheel'. The distribution package has this already set up for you. However, if you accidentally extract the archive as a normal user (eg. omit 'sudo su' before unpacking the tarball), permissions might become corrupted and the loading of the driver will fail. In this case you can correct the problem by running 'sudo chown -R root:wheel ./pmem.kext' from within the 'OSXPMem' directory.
  1. If you try to run the imager from NFS or another networked file-system, permissions might also become corrupted. If the imager reports a failure to load the pmem driver, check the drivers permissions. If it is not owned by user 'root' and group 'wheel' and step 1 can't correct this, try copying it somewhere else and correct permissions there.

## Compatibility ##

Due to the nature of physical memory access many things are very platform
dependent. The tool is designed to work on 64 bit Intel Macs. It can probably be compiled to work in 32 bit mode, the binary distribution however only contains 64 bit binaries.

Several low-level api's have changed in recent OS X versions. We have tested the imager and driver on OS X 10.7 and 10.8, on which they work flawlessly. It should also work on 10.6, but might encounter problems unloading the driver, as the unloading api in IOKit is new in 10.7.

We have also successfully tested the tool in a VMWare Fusion OS X 10.7 machine, so it should work in virtualized environments.

## Building from Source ##

The project can only be built on Mac OS X, due to linking with Apple
libraries and kernel headers. You also need the Apple Developer Kit installed, the easiest way to obtain this is by installing XCode.

Build the kext and imager with the provided Makefile by running 'make'. This will create a directory './build' and place the resulting binaries there.

There is also a bash script called 'package.sh', which will build the
kernel extension and the imager, and package them into a compressed tarball.

You might need to adjust the 'SDK\_PATH' and 'FRAMEWORK\_PATH' in the Makefile, if they reside in different locations on your build system.