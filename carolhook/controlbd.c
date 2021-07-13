/*
There must be pen type controller on COM11 (which is "controller board" in game).
Otherwise ERROR 3002 happens and it won't progress further.

TX Buffer: e0 11 01 01 10 23
*/
#include <windows.h>

#include <assert.h>
#include <process.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "board/slider-cmd.h"
#include "board/slider-frame.h"

#include "carolhook/carol-dll.h"
#include "carolhook/controlbd.h"

#include "hook/iobuf.h"
#include "hook/iohook.h"

#include "hooklib/uart.h"

#include "util/dprintf.h"
#include "util/dump.h"

static HRESULT controlbd_handle_irp(struct irp *irp);
static HRESULT controlbd_handle_irp_locked(struct irp *irp);

static HRESULT controlbd_req_dispatch(const union slider_req_any *req);
static HRESULT controlbd_req_reset(void);
static HRESULT controlbd_req_get_board_info(void);

static CRITICAL_SECTION controlbd_lock;
static struct uart controlbd_uart;
static uint8_t controlbd_written_bytes[520];
static uint8_t controlbd_readable_bytes[520];

HRESULT controlbd_hook_init(const struct controlbd_config *cfg)
{
    assert(cfg != NULL);
    assert(carol_dll.controlbd_init != NULL);

    if (!cfg->enable) {
        return S_FALSE;
    }

    InitializeCriticalSection(&controlbd_lock);

    uart_init(&controlbd_uart, 11);
    controlbd_uart.written.bytes = controlbd_written_bytes;
    controlbd_uart.written.nbytes = sizeof(controlbd_written_bytes);
    controlbd_uart.readable.bytes = controlbd_readable_bytes;
    controlbd_uart.readable.nbytes = sizeof(controlbd_readable_bytes);

    return iohook_push_handler(controlbd_handle_irp);
}

static HRESULT controlbd_handle_irp(struct irp *irp)
{
    HRESULT hr;

    assert(irp != NULL);

    if (!uart_match_irp(&controlbd_uart, irp)) {
        return iohook_invoke_next(irp);
    }

    EnterCriticalSection(&controlbd_lock);
    hr = controlbd_handle_irp_locked(irp);
    LeaveCriticalSection(&controlbd_lock);

    return hr;
}

static HRESULT controlbd_handle_irp_locked(struct irp *irp)
{
    union slider_req_any req;
    struct iobuf req_iobuf;
    HRESULT hr;

    if (irp->op == IRP_OP_OPEN) {
        dprintf("Wonderland Wars control board: Starting backend\n");
        hr = carol_dll.controlbd_init();

        if (FAILED(hr)) {
            dprintf("Wonderland Wars control board: Backend error: %x\n", (int) hr);

            return hr;
        }
    }

    hr = uart_handle_irp(&controlbd_uart, irp);

    if (FAILED(hr) || irp->op != IRP_OP_WRITE) {
        return hr;
    }

    for (;;) {
#if 1
        dprintf("TX Buffer:\n");
        dump_iobuf(&controlbd_uart.written);
#endif

        req_iobuf.bytes = req.bytes;
        req_iobuf.nbytes = sizeof(req.bytes);
        req_iobuf.pos = 0;

        hr = slider_frame_decode(&req_iobuf, &controlbd_uart.written);

        if (hr != S_OK) {
            if (FAILED(hr)) {
                dprintf("Wonderland Wars control board: Deframe error: %x\n", (int) hr);
            }

            return hr;
        }

#if 1
        dprintf("Deframe Buffer:\n");
        dump_iobuf(&req_iobuf);
#endif

        hr = controlbd_req_dispatch(&req);

        if (FAILED(hr)) {
            dprintf("Wonderland Wars control board: Processing error: %x\n", (int) hr);
        }
    }
}

static HRESULT controlbd_req_dispatch(const union slider_req_any *req)
{
    switch (req->hdr.cmd) {
    case SLIDER_CMD_RESET:
        return controlbd_req_reset();

    case SLIDER_CMD_GET_BOARD_INFO:
        return controlbd_req_get_board_info();

    default:
        dprintf("Unhandled command %02x\n", req->hdr.cmd);

        return S_OK;
    }
}

static HRESULT controlbd_req_reset(void)
{
    struct slider_hdr resp;

    dprintf("Wonderland Wars control board: Reset\n");

    resp.sync = 0xFF;
    resp.cmd = SLIDER_CMD_RESET;
    resp.nbytes = 0;

    return slider_frame_encode(&controlbd_uart.readable, &resp, sizeof(resp));
}

static HRESULT controlbd_req_get_board_info(void)
{
    struct slider_resp_get_board_info resp;

    dprintf("Wonderland Wars control board: Get firmware version\n");

    memset(&resp, 0, sizeof(resp));
    resp.hdr.sync = SLIDER_FRAME_SYNC;
    resp.hdr.cmd = SLIDER_CMD_GET_BOARD_INFO;
    resp.hdr.nbytes = sizeof(resp.version);

    strcpy_s(
            resp.version,
            sizeof(resp.version),
            "15312   \xA0" "06699\xFF" "\x90");

    return slider_frame_encode(&controlbd_uart.readable, &resp, sizeof(resp));
}
