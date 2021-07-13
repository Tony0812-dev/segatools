#include <windows.h>

#include <process.h>
#include <stdbool.h>
#include <stdint.h>

#include "carolio/carolio.h"
#include "carolio/config.h"

static unsigned int __stdcall carol_io_controlbd_thread_proc(void *ctx);

static bool carol_io_coin;
static uint16_t carol_io_coins;
static HANDLE carol_io_controlbd_thread;
static bool carol_io_controlbd_stop_flag;
static struct carol_io_config carol_io_cfg;

uint16_t carol_io_get_api_version(void)
{
    return 0x0100;
}

HRESULT carol_io_jvs_init(void)
{
    carol_io_config_load(&carol_io_cfg, L".\\segatools.ini");

    return S_OK;
}

void carol_io_jvs_read_coin_counter(uint16_t *out)
{
    if (out == NULL) {
        return;
    }

    if (GetAsyncKeyState(carol_io_cfg.vk_coin)) {
        if (!carol_io_coin) {
            carol_io_coin = true;
            carol_io_coins++;
        }
    } else {
        carol_io_coin = false;
    }

    *out = carol_io_coins;
}

void carol_io_jvs_poll(uint8_t *opbtn, uint8_t *beams)
{
    //size_t i;

    if (GetAsyncKeyState(carol_io_cfg.vk_test)) {
        *opbtn |= 0x01; /* Test */
    }

    if (GetAsyncKeyState(carol_io_cfg.vk_service)) {
        *opbtn |= 0x02; /* Service */
    }

/*
    if (GetAsyncKeyState(carol_io_cfg.vk_ir)) {
        if (carol_io_hand_pos < 6) {
            carol_io_hand_pos++;
        }
    } else {
        if (carol_io_hand_pos > 0) {
            carol_io_hand_pos--;
        }
    }

    for (i = 0 ; i < 6 ; i++) {
        if (carol_io_hand_pos > i) {
            *beams |= (1 << i);
        }
    }
    */
}

HRESULT carol_io_controlbd_init(void)
{
    return S_OK;
}

void carol_io_controlbd_start(carol_io_controlbd_callback_t callback)
{
    if (carol_io_controlbd_thread != NULL) {
        return;
    }

    carol_io_controlbd_thread = (HANDLE) _beginthreadex(
            NULL,
            0,
            carol_io_controlbd_thread_proc,
            callback,
            0,
            NULL);
}

void carol_io_controlbd_stop(void)
{
    if (carol_io_controlbd_thread == NULL) {
        return;
    }

    carol_io_controlbd_stop_flag = true;

    WaitForSingleObject(carol_io_controlbd_thread, INFINITE);
    CloseHandle(carol_io_controlbd_thread);
    carol_io_controlbd_thread = NULL;
    carol_io_controlbd_stop_flag = false;
}

static unsigned int __stdcall carol_io_controlbd_thread_proc(void *ctx)
{
    carol_io_controlbd_callback_t callback;
    uint8_t pressure[32];
    //size_t i;

    callback = ctx;

    while (!carol_io_controlbd_stop_flag) {
        callback(pressure);
        Sleep(1);
    }

    return 0;
}