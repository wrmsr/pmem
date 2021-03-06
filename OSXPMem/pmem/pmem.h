// Creates the device '/dev/pmem',
// which allows direct read access to physical memory on OSX.
//
// IMPORTANT:
// This code has only been tested on OSX 10.7 with a 64 bit kernel
// Parts of it are architecture dependent and won't necessarily work
// on a 32 bit kernel.
//
// Copyright 2012 Google Inc. All Rights Reserved.
// Author: Johannes Stüttgen (johannes.stuettgen@gmail.com)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _VOLATILITY_DRIVER_PMEM_H_
#define _VOLATILITY_DRIVER_PMEM_H_

// ioctl commands for this driver
#include "pmem_ioctls.h"
// variable length function arguments for the logging functions
#include <stdarg.h>
// sysctl and general kernel tools
#include <libkern/OSMalloc.h>
#include <mach/mach_types.h>
#include <sys/sysctl.h>
#include <sys/systm.h>
// Character device dependencies
#include <miscfs/devfs/devfs.h>
#include <sys/conf.h>
#include <sys/param.h>
#include <sys/uio.h>
// Physical memory types
#include <mach/vm_param.h>
#include <mach/vm_types.h>
#include <sys/vm.h>
#include <vm/vm_kern.h>
// Kernel boot arguments from EFI
#include <pexpert/i386/boot.h>
#include <pexpert/pexpert.h>
// IOKit memory interface
#include <IOKit/IOMemoryDescriptor.h>
// Tasks and threads (for kernel_task)
#include <kern/task.h>

// The platform expert keeps track of the EFI boot arguments
// and thus the physical memory map.
extern PE_state_t PE_state;

// The Kernel Memory Map.
// A single physical memory segment in the memory map.
typedef struct pmap_memory_regions {
        ppnum_t base;
        ppnum_t end;
        ppnum_t alloc;
        uint32_t type;
        uint64_t attribute;
} pmap_memory_region_t;
// This is where the kernel stores the physical memory map.
extern pmap_memory_region_t pmap_memory_regions[];
extern unsigned pmap_memory_region_count;

// Kernel memory management.
// We don't need to access this struct, so this is enough.
typedef struct pmap *pmap_t;
// These symbols are not in the kernel headers,
// they will be linked from com.apple.kpi.unsupported by the loader.
extern pmap_t kernel_pmap;
extern vm_map_t kernel_map;
extern ppnum_t pmap_find_phys(pmap_t pmap, addr64_t va);
extern void bcopy_phys(addr64_t from, addr64_t to, vm_size_t bytes);

// Read as many bytes as requested in uio->resid (starting from uio->offset)
// and copy them to the userspace buffer in the uio.
static kern_return_t pmem_read(dev_t dev, struct uio *uio, __unused int rw);

// Read directly from physical memory.
static kern_return_t pmem_read_memory(struct uio *uio);

// Will read as many bytes as possible from physical memory,
// stopping at page boundaries or invalid regions like memory-mapped I/O.
static uint64_t pmem_partial_read(struct uio *uio, addr64_t addr,
                                  uint64_t requested_bytes);

// User-mode interface to obtain the binary memory map
static kern_return_t pmem_ioctl(dev_t dev, u_long cmd, caddr_t data, int flag,
                                struct proc *p);

// Determine the physical address for a kernel virtual address.
static addr64_t pmem_kernel_virt_to_phys(addr64_t addr);

// Debug logging, only active in debug build.
static void pmem_log(const char *fmt, ...);

// Error logging, always active.
static void pmem_error(const char *fmt, ...);

// Determine if a physical address is accessible by the kernel,
// meaning it offsets into a valid page frame.
static boolean_t pmem_page_valid(addr64_t page);

// Try to free all resources.
static int pmem_cleanup(int error);

// Grab the physical memory map and fill the pmap buffer.
static void pmem_init_pmap(void);

// Declare c calling conventions for the linker to find these symbols
extern "C" {
  kern_return_t pmem_start(kmod_info_t * ki, void *d);
  kern_return_t pmem_stop(kmod_info_t *ki, void *d);
}

#endif  // _VOLATILITY_DRIVER_PMEM_H_
