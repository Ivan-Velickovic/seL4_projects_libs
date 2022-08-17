#include <sel4vmmplatsupport/drivers/virtio_pci_emul.h>

// @ivanv:
// - get dataport working and see if we can at least intercept a packet.
// - finish implement initialisation
// - implement rx and tx.

/* Note that this implements v1.1 of the virtIO specification */
// @ivanv more detail here, where to find it etc

typedef struct vsockif_virtio_emul_internal {
    struct vsock_driver driver;
    struct virtio_vsock_config config;
    ps_dma_man_t dma_man;
} vsockif_virtio_emul_internal_t;

static bool is_valid_guest_cid(uint64_t guest_cid) {
    /* These are the reserved/invalid CIDs, see 5.10.4. */
    switch (guest_cid) {
        case 0:
        case 1:
        case 2:
        case 0xffffffff:
        case 0xffffffffffffffff:
            return false;
        default:
            return true;
    }
}

static void emul_rx_complete(void *iface, char *buf, unsigned int len)
{
    // virtio_emul_t *emul = (virtio_emul_t *)iface;
    // console_internal_t *con = emul->internal;
    // vqueue_t *virtq = &emul->virtq;
    // int i;
    // struct vring *vring = &virtq->vring[RX_QUEUE];

    // uint16_t guest_idx = ring_avail_idx(emul, vring);
    // uint16_t idx = virtq->last_idx[RX_QUEUE];

    // // @ivanv: what is this doing
    // if (idx != guest_idx) {

    // }
}

static int virtio_vsock_emul_event(struct vsock_driver *driver, enum virtio_vsock_event_id id)
{
    int status = 0;  /* Variable to ensure transfer succeeded */

    switch (id) {
    case VIRTIO_VSOCK_EVENT_TRANSPORT_RESET:
        // Indicates that communication has been interrupted.
        // Shutdown connection, guest_cid is fetched again.
    default:
        ZF_LOGE("virtio_vsock: Invalid event id (%d)", id);
        break;
    }

    // return (status != 0 ? VIRTIO_BLK_XFER_COMPLETE : VIRTIO_BLK_XFER_FAILED);
}

static int virtio_vsock_emul_transfer(struct vsock_driver *driver, enum virtio_vsock_op type)
{
    printf("VSOCK|INFO: in virtio_vsock_emul_transfer\n");
    int status = 0;  /* Variable to ensure transfer succeeded */

    switch (type) {
    case VIRTIO_VSOCK_OP_REQUEST:
        // If a listening socket exists on the destination, send
        // a VIRTIO_VSOCK_OP_RESPONSE.
        // Establish the connection.
        // If there isn't a listening socket or there isn't enough
        // resources to establish a connection, send a VIRTIO_VSOCK_OP_RST.
        break;
    case VIRTIO_VSOCK_OP_RESPONSE:
        break;
    case VIRTIO_VSOCK_OP_RST:
        // Aborts the connection process or forcibly disconnects from a connected
        // socket.
        break;
    case VIRTIO_VSOCK_OP_SHUTDOWN:
        // Read flags field
        // If bit 0 is set, this indicates the peer will not receive any more
        // data.
        // If bit 1 is set, this indicates the peer will not send any more data.
        // Note that these hints are permanent. Successive packets with these bits
        // cleared do not reset the hints.

        // Getting one OR MORE shutdown packets indicate no more data will be sent
        // and received, followed by a RST response from the peer. If no RST response
        // is received within a specific amount of time, a RST packet is sent to forcibly
        // disconnect the socket.
        break;
    case VIRTIO_VSOCK_OP_RW:
        // These packets must only be transferred when the peer has sufficient free buffer
        // space for the payload.
        break;
    case VIRTIO_VSOCK_OP_CREDIT_UPDATE:
        break;
    case VIRTIO_VSOCK_OP_CREDIT_REQUEST:
        break;
    default:
        // If we get a packet with an unknown request type, we have to send a
        // VIRTIO_VSOCK_OP_RST
        ZF_LOGE("virtio_vsock: Invalid request type (%d)", type);
        break;
    }

    // return (status != 0 ? VIRTIO_BLK_XFER_COMPLETE : VIRTIO_BLK_XFER_FAILED);
}

static int virtio_vsock_emul_receive(struct vsock_driver *driver, enum virtio_vsock_op type)
{
    printf("VSOCK|INFO: in virtio_vsock_emul_receive\n");
    int status = 0;  /* Variable to ensure transfer succeeded */

    switch (type) {
    default:
        // If we get a packet with an unknown request type, we have to send a
        // VIRTIO_VSOCK_OP_RST
        ZF_LOGE("virtio_vsock: Invalid request type (%d)", type);
        break;
    }

    // return (status != 0 ? VIRTIO_BLK_XFER_COMPLETE : VIRTIO_BLK_XFER_FAILED);
}

static void handle_virtio_vsock_request(virtio_emul_t *emul) {
    printf("VSOCK|INFO: in handle_virtio_vsock_request\n");

    // vsockif_virtio_emul_internal_t *vsock = (ethif_internal_t *)emul->internal;
    // vqueue_t *vq = &emul->virtq;
    // int i;
    // struct vring *vring = &vq->vring[RX_QUEUE];

    // /* grab the next receive chain */
    // struct virtio_vsock_hdr virtio_hdr;
    // memset(&virtio_hdr, 0, sizeof(virtio_hdr));
    // uint16_t guest_idx = ring_avail_idx(emul, vring);
    // uint16_t idx = vq->last_idx[RX_QUEUE];
    // if (idx != guest_idx) {
    //     /* total length of the written packet so far */
    //     size_t tot_written = 0;
    //     /* amount of the current descriptor written */
    //     size_t desc_written = 0;
    //     /* how much we have written of the current buffer */
    //     size_t buf_written = 0;
    //     /* the current buffer. -1 indicates the virtio vsock buffer */
    //     int current_buf = -1;
    //     uint16_t desc_head = ring_avail(emul, vring, idx);
    //     /* start walking the descriptors */
    //     struct vring_desc desc;
    //     uint16_t desc_idx = desc_head;
    //     do {
    //         desc = ring_desc(emul, vring, desc_idx);
    //         /* determine how much we can copy */
    //         uint32_t copy;
    //         void *buf_base = NULL;
    //         if (current_buf == -1) {
    //             copy = sizeof(struct virtio_vsock_hdr) - buf_written;
    //             buf_base = &virtio_hdr;
    //         } else {
    //             copy = lens[current_buf] - buf_written;
    //             buf_base = cookies[current_buf];
    //         }
    //         copy = MIN(copy, desc.len - desc_written);
    //         vm_guest_write_mem(emul->vm, buf_base + buf_written, (uintptr_t)desc.addr + desc_written, copy);
    //         /* update amounts */
    //         tot_written += copy;
    //         desc_written += copy;
    //         buf_written += copy;
    //         /* see what's gone over */
    //         if (desc_written == desc.len) {
    //             if (!desc.flags & VRING_DESC_F_NEXT) {
    //                 /* descriptor chain is too short to hold the whole packet.
    //                  * just truncate */
    //                 break;
    //             }
    //             desc_idx = desc.next;
    //             desc_written = 0;
    //         }
    //         if (current_buf == -1) {
    //             if (buf_written == sizeof(struct virtio_vsock_hdr)) {
    //                 current_buf++;
    //                 buf_written = 0;
    //             }
    //         } else {
    //             if (buf_written == lens[current_buf]) {
    //                 current_buf++;
    //                 buf_written = 0;
    //             }
    //         }
    //     } while (current_buf < num_bufs);
    //     /* now put it in the used ring */
    //     struct vring_used_elem used_elem = {desc_head, tot_written};
    //     ring_used_add(emul, vring, used_elem);

    //     /* record that we've used this descriptor chain now */
    //     vq->last_idx[RX_QUEUE]++;
    //     /* notify the guest that there is something in its used ring */
    //     net->driver.i_fn.raw_handleIRQ(&net->driver, 0);
    // }
    // for (i = 0; i < num_bufs; i++) {
    //     ps_dma_unpin(&net->dma_man, cookies[i], BUF_SIZE);
    //     ps_dma_free(&net->dma_man, cookies[i], BUF_SIZE);
    // }
}

static bool vsock_device_emul_io_in(struct virtio_emul *emul, unsigned int offset, unsigned int size, unsigned int *result)
{
    printf("VSOCK|INFO: in vsock_device_emul_io_in\n");
    bool handled = false;
    vsockif_virtio_emul_internal_t *vsockif_internal = emul->internal;
    switch (offset) {
    case VIRTIO_PCI_HOST_FEATURES:
        handled = true;
        assert(size == 4);
        /* There are no feature bits for virtIO sock. */
        *result = 0;
        break;
    case VIRTIO_PCI_CONFIG_OFF(0) ... VIRTIO_PCI_CONFIG_OFF(0) + sizeof(struct virtio_vsock_config):
        // @ivanv: I don't understand this above line
        handled = true;
        assert(size == 1);
        printf("VSOCK|INFO: result is: %d\n", *result);
        memcpy(result, (((uint8_t *)&vsockif_internal->config) + offset - VIRTIO_PCI_CONFIG_OFF(0)), size);
        break;
    default:
        ZF_LOGE("something wrong? offset is: %d\n", offset);
    }

    return handled;
}

static bool vsock_device_emul_io_out(struct virtio_emul *emul, unsigned int offset, unsigned int size, unsigned int value)
{
    printf("VSOCK|INFO: in vsock_device_emul_io_out\n");
    bool handled = false;
    vsockif_virtio_emul_internal_t *vsockif_internal = emul->internal;
    switch (offset) {
    case VIRTIO_PCI_GUEST_FEATURES:
        handled = true;
        assert(size == 4);
        /* There are no feature bits for virtIO sock. */
        assert(value == 0);
        break;
    case VIRTIO_PCI_QUEUE_NOTIFY:
        handled = true;
        handle_virtio_vsock_request(emul);
    default:
        ZF_LOGE("something wrong? offset is: %d\n", offset);
    }

    return handled;
}

static void vsock_emul_notify(virtio_emul_t *emul)
{
    printf("VSOCK|INFO: in emul notify\n");
    if (emul->virtq.status != VIRTIO_CONFIG_S_DRIVER_OK) {
        ZF_LOGW("Driver is not happy\n"); // @ivanv
        return;
    }
    handle_virtio_vsock_request(emul);
}

void *vsock_virtio_emul_init(virtio_emul_t *emul, ps_io_ops_t io_ops, vsock_driver_init driver, void *config)
{
    ZF_LOGE("VSOCK|INFO: Initialising virtIO vsock emul\n");
    vsockif_virtio_emul_internal_t *internal = calloc(1, sizeof(*internal));
    if (!internal) {
        goto error;
    }

    emul->notify = vsock_emul_notify;
    emul->device_io_in = vsock_device_emul_io_in;
    emul->device_io_out = vsock_device_emul_io_out;

    // internal->driver.cb_cookie = emul;
    // internal->driver.i_cb = emul_callbacks;
    // internal->dma_man = io_ops.dma_manager;

    int err = driver(&internal->driver, io_ops, config);
    if (err) {
        ZF_LOGE("Failed to initialize driver");
        goto error;
    }

    return (void *)internal;
error:
    if (emul) {
        free(emul);
    }
    if (internal) {
        free(internal);
    }
    return NULL;
}
