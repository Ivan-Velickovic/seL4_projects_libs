#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <platsupport/io.h>

#include <sel4vmmplatsupport/drivers/virtio.h>
#include <sel4vmmplatsupport/drivers/virtio_vsock.h>

#include <pci/helper.h>
#include <sel4vmmplatsupport/drivers/pci_helper.h>

// @ivanv: I have no idea what this should actually be.
#define QUEUE_SIZE 128

static ps_io_ops_t ops;

static int emul_driver_init(struct vsock_driver *driver, ps_io_ops_t io_ops, void *config)
{
    // virtio_sock_t *sock = (virtio_sock_t *)config;
    // driver->disk_data = config;
    // driver->dma_alignment = sizeof(uintptr_t);
    // driver->i_fn = sock->emul_driver_funcs;
    // sock->emul_driver = driver;
    // @ivanv: todo

    return 0;
}

static vmm_pci_entry_t vmm_virtio_vsock_pci_bar(unsigned int iobase, size_t iobase_size_bits,
                                              unsigned int interrupt_pin, unsigned int interrupt_line)
{
    vmm_pci_device_def_t *pci_config;
    int err = ps_calloc(&ops.malloc_ops, 1, sizeof(*pci_config), (void **) &pci_config);
    ZF_LOGF_IF(err, "Failed to allocate pci_config");
    *pci_config = (vmm_pci_device_def_t) {
        .vendor_id = VIRTIO_PCI_VENDOR_ID,
        .device_id = VIRTIO_VSOCK_PCI_DEVICE_ID,
        .command = PCI_COMMAND_IO,
        .header_type = PCI_HEADER_TYPE_NORMAL,
        .subsystem_vendor_id = VIRTIO_PCI_SUBSYSTEM_VENDOR_ID,
        .subsystem_id = VIRTIO_ID_VSOCK,
        .interrupt_pin = interrupt_pin,
        .interrupt_line = interrupt_line,
        .bar0 = iobase | PCI_BASE_ADDRESS_SPACE_IO,
        .cache_line_size = 64,
        .latency_timer = 64,
        .prog_if = VIRTIO_PCI_CLASS_VSOCK & 0xff, // @ivanv: don't know why these are here, they should include a comment
        .subclass = (VIRTIO_PCI_CLASS_VSOCK >> 8) & 0xff,
        .class_code = (VIRTIO_PCI_CLASS_VSOCK >> 16) & 0xff,
    };
    vmm_pci_entry_t entry = (vmm_pci_entry_t) {
        .cookie = pci_config,
        .ioread = vmm_pci_mem_device_read,
        .iowrite = vmm_pci_entry_ignore_write
    };

    vmm_pci_bar_t bars[1] = {{
            .mem_type = NON_MEM,
            .address = iobase,
            .size_bits = iobase_size_bits
        }
    };
    vmm_pci_entry_t virtio_pci_bar;
    // if (emulate_bar_access) {
    //     virtio_pci_bar = vmm_pci_create_bar_emulation(entry, 1, bars);
    // } else {
    //     virtio_pci_bar = vmm_pci_create_passthrough_bar_emulation(entry, 1, bars);
    // }
    // @ivanv: difference between this and passthrough bar emulation?
    return vmm_pci_create_bar_emulation(entry, 1, bars);
}

static int virtio_vsock_io_in(void *cookie, unsigned int port_no, unsigned int size, unsigned int *result)
{
    virtio_vsock_t *vsock = (virtio_vsock_t *)cookie;
    unsigned int offset = port_no - vsock->iobase;
    unsigned int val;
    int err = vsock->emul->io_in(vsock->emul, offset, size, &val);
    if (err) {
        return err;
    }
    *result = val;

    return 0;
}

static int virtio_vsock_io_out(void *cookie, unsigned int port_no, unsigned int size, unsigned int value)
{
    int ret;
    virtio_vsock_t *vsock = (virtio_vsock_t *)cookie;
    unsigned int offset = port_no - vsock->iobase;
    ret = vsock->emul->io_out(vsock->emul, offset, size, value);
    return ret;
}

virtio_vsock_t *common_make_virtio_vsock(vm_t *vm, vmm_pci_space_t *pci, vmm_io_port_list_t *ioport,
                                     ioport_range_t ioport_range, ioport_type_t port_type, unsigned int interrupt_pin, unsigned int interrupt_line)
{
    int err = ps_new_stdlib_malloc_ops(&ops.malloc_ops);
    ZF_LOGF_IF(err, "Failed to get malloc ops");

    virtio_vsock_t *vsock;
    err = ps_calloc(&ops.malloc_ops, 1, sizeof(*vsock), (void **)&vsock);
    ZF_LOGF_IF(err, "Failed to allocate virtio vsock");

    ioport_interface_t virtio_io_interface = {vsock, virtio_vsock_io_in, virtio_vsock_io_out, "VIRTIO VSOCK"};
    ioport_entry_t *io_entry = vmm_io_port_add_handler(ioport, ioport_range, virtio_io_interface, port_type);
    if (!io_entry) {
        ZF_LOGE("Failed to add vmm io port handler");
        return NULL;
    }

    size_t iobase_size_bits = BYTES_TO_SIZE_BITS(io_entry->range.size);
    vsock->iobase = io_entry->range.start;

    vmm_pci_entry_t entry = vmm_virtio_vsock_pci_bar(io_entry->range.start, iobase_size_bits, interrupt_pin, interrupt_line);
    vmm_pci_add_entry(pci, entry, NULL);

    ps_io_ops_t ioops;
    // ioops.dma_manager = (ps_dma_man_t) {
    //     .cookie = NULL,
    //     .dma_alloc_fn = malloc_dma_alloc,
    //     .dma_free_fn = malloc_dma_free,
    //     .dma_pin_fn = malloc_dma_pin,
    //     .dma_unpin_fn = malloc_dma_unpin,
    //     .dma_cache_op_fn = malloc_dma_cache_op
    // };

    // @ivanv: need to somehow pass in the interface functions/backend
    // vsock->emul_driver = backend;
    vsock->emul = virtio_emul_init(ioops, QUEUE_SIZE, vm, emul_driver_init, vsock, VIRTIO_VSOCK);
    assert(vsock->emul);

    return vsock;
}
