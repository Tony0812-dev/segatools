#include <windows.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "board/io4.h"

#include "cmhook/cm-dll.h"

#include "util/dprintf.h"

static HRESULT cm_io4_poll(void *ctx, struct io4_state *state);
static uint16_t coins;

static const struct io4_ops cm_io4_ops = {
    .poll = cm_io4_poll,
};

HRESULT cm_io4_hook_init(const struct io4_config *cfg)
{
    HRESULT hr;

    assert(cm_dll.init != NULL);

    hr = io4_hook_init(cfg, &cm_io4_ops, NULL);

    if (FAILED(hr)) {
        return hr;
    }

    return cm_dll.init();
}

static HRESULT cm_io4_poll(void *ctx, struct io4_state *state)
{
    uint8_t opbtn;
    HRESULT hr;

    assert(cm_dll.poll != NULL);
    assert(cm_dll.get_opbtns != NULL);

    memset(state, 0, sizeof(*state));

    hr = cm_dll.poll();

    if (FAILED(hr)) {
        return hr;
    }

    opbtn = 0;

    cm_dll.get_opbtns(&opbtn);

    if (opbtn & cm_IO_OPBTN_TEST) {
        state->buttons[0] |= IO4_BUTTON_TEST;
    }

    if (opbtn & cm_IO_OPBTN_SERVICE) {
        state->buttons[0] |= IO4_BUTTON_SERVICE;
    }

    if (opbtn & cm_IO_OPBTN_COIN) {
        coins++;
    }
    state->chutes[0] = coins << 8;

    return S_OK;
}
