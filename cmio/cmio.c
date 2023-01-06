#include <windows.h>
#include <xinput.h>

#include <limits.h>
#include <stdint.h>

#include "cmio/cmio.h"
#include "cmio/config.h"

static uint8_t cm_opbtn;
static struct cm_io_config cm_io_cfg;
static bool cm_io_coin;

uint16_t cm_io_get_api_version(void)
{
    return 0x0100;
}

HRESULT cm_io_init(void)
{
    cm_io_config_load(&cm_io_cfg, L".\\segatools.ini");
    return S_OK;
}

HRESULT cm_io_poll(void)
{
    cm_opbtn = 0;

    if (GetAsyncKeyState(cm_io_cfg.vk_test) & 0x8000) {
        cm_opbtn |= cm_IO_OPBTN_TEST;
    }

    if (GetAsyncKeyState(cm_io_cfg.vk_service) & 0x8000) {
        cm_opbtn |= cm_IO_OPBTN_SERVICE;
    }

    if (GetAsyncKeyState(cm_io_cfg.vk_coin) & 0x8000) {
        if (!cm_io_coin) {
            cm_io_coin = true;
            cm_opbtn |= cm_IO_OPBTN_COIN;
        }
    } else {
        cm_io_coin = false;
    }

    return S_OK;
}

void cm_io_get_opbtns(uint8_t *opbtn)
{
    if (opbtn != NULL) {
        *opbtn = cm_opbtn;
    }
}
