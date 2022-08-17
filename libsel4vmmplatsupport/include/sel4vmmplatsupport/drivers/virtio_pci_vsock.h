/* @ivanv: should this file exist? */

// @ivnav: vsock_driver state might look something like this, not quite sure yet.
// typedef void (*vsock_handle_irq_fn_t)(void *cookie);

#pragma once

#define MAX_DEVICE 2

struct vsock_driver {
    void *vsock_data;
    // struct raw_iface_funcs i_fn;
    // struct raw_iface_callbacks i_cb;
    uint64_t guest_cids[MAX_DEVICE];
    ps_io_ops_t io_ops;
};

typedef int (*vsock_driver_init)(struct vsock_driver *driver, ps_io_ops_t io_ops, void *config);
