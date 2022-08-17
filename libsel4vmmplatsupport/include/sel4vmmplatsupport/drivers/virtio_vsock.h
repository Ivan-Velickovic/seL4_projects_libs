// @ivanv: license


#pragma once

#include <sel4vm/guest_vm.h>

#include <sel4vmmplatsupport/ioports.h>
#include <sel4vmmplatsupport/drivers/pci.h>
#include <sel4vmmplatsupport/drivers/virtio_pci_emul.h>

// @ivanv I guess I'm kinda confused as to what this is vs the driver state?
typedef struct virtio_vsock {
    unsigned int iobase;
    virtio_emul_t *emul;
    struct vsock_driver *emul_driver;
    // raw_diskiface_funcs_t emul_driver_funcs;
    ps_io_ops_t ioops;
} virtio_vsock_t;

virtio_vsock_t *common_make_virtio_vsock(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *ioport,
                                     ioport_range_t ioport_range, ioport_type_t port_type, unsigned int interrupt_pin, unsigned int interrupt_line);

