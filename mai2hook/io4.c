#include <windows.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "board/io4.h"

#include "util/dprintf.h"

static HRESULT mai2_io4_poll(void *ctx, struct io4_state *state);

static const struct io4_ops mai2_io4_ops = {
    .poll = mai2_io4_poll,
};

HRESULT mai2_io4_hook_init(void)
{
    HRESULT hr;

    hr = io4_hook_init(&mai2_io4_ops, NULL);

    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

static HRESULT mai2_io4_poll(void *ctx, struct io4_state *state)
{

    /* Disables all the inputs, this makes debug input work */

    memset(state, 0, sizeof(*state));

    if (GetAsyncKeyState('1') & 0x8000) {
        state->buttons[0] |= IO4_BUTTON_TEST;
    }

    if (GetAsyncKeyState('2') & 0x8000) {
        state->buttons[0] |= IO4_BUTTON_SERVICE;
    }

    state->buttons[0] |= 1;
    state->buttons[1] |= 1;

    /*
    if (GetAsyncKeyState('A') & 0x8000) {
        --state->buttons[0];
    }
    */

    for (int i = 2; i <= 3; ++i) {
        state->buttons[0] |= 1 << i;
        state->buttons[1] |= 1 << i;
        /*
        if (GetAsyncKeyState(64 + i) & 0x8000) {
            state->buttons[0] -= 1 << i;
        }
        */
    }

    for (int j = 11; j <= 15; ++j) {
        state->buttons[0] |= 1 << j;
        state->buttons[1] |= 1 << j;
        /*
        if (GetAsyncKeyState(57 + j) & 0x8000) {
            state->buttons[0] -= 1 << j;
        }
        */
    }

    //state = 0x7FFF;

    return S_OK;
}
