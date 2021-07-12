/*
This part (touch screen hook) is mostly taken from spicetools, which is GPL.
This means there can be some license issues if you do use this code in some other places without including source code.
*/
#include <windows.h>

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "hook/com-proxy.h"
#include "hook/table.h"

#include "hooklib/config.h"
#include "hooklib/dll.h"
#include "hooklib/touch.h"

#include "util/dprintf.h"

/* API hooks */

static int WINAPI hook_GetSystemMetrics(
    int nIndex
);

static BOOL WINAPI hook_RegisterTouchWindow(
    HWND hwnd,
    ULONG ulFlags
);

static BOOL WINAPI hook_GetTouchInputInfo(
    HANDLE hTouchInput,
    UINT cInputs,
    PTOUCHINPUT pInputs,
    int cbSize
);

/* Link pointers */

static int (WINAPI *next_GetSystemMetrics)(
    int nIndex
);

static BOOL (WINAPI *next_RegisterTouchWindow)(
    HWND hwnd,
    ULONG ulFlags
);

static BOOL (WINAPI *next_GetTouchInputInfo)(
    HANDLE hTouchInput,
    UINT cInputs,
    PTOUCHINPUT pInputs,
    int cbSize
);

static bool touch_hook_initted;
static struct touch_config touch_config;

static const struct hook_symbol touch_hooks[] = {
    {
        .name   = "GetSystemMetrics",
        .patch  = hook_GetSystemMetrics,
        .link   = (void **) &next_GetSystemMetrics
    },
    {
        .name   = "RegisterTouchWindow",
        .patch  = hook_RegisterTouchWindow,
        .link   = (void **) &next_RegisterTouchWindow
    },
    {
        .name   = "GetTouchInputInfo",
        .patch  = hook_GetTouchInputInfo,
        .link   = (void **) &next_GetTouchInputInfo
    },
};

void touch_hook_init(const struct touch_config *cfg, HINSTANCE self)
{
    assert(cfg != NULL);

    if (!cfg->enable) {
        return;
    }

    if (touch_hook_initted) {
        return;
    }

    touch_hook_initted = true;

    memcpy(&touch_config, cfg, sizeof(*cfg));
    hook_table_apply(NULL, "user32.dll", touch_hooks, _countof(touch_hooks));
    dprintf("TOUCH: hook enabled.\n");
}

// Spicetools misc/wintouchemu.cpp

static int WINAPI hook_GetSystemMetrics(
    int nIndex
)
{
    if (nIndex == 94) return 0x01 | 0x02 | 0x40 | 0x80;

    int orig = next_GetSystemMetrics(nIndex);

    return orig;
}

static BOOL WINAPI hook_RegisterTouchWindow(
    HWND hwnd,
    ULONG ulFlags
)
{
    return true;
}

// Converting mouse event to touch event
// Does not work at current stage
static BOOL WINAPI hook_GetTouchInputInfo(
    HANDLE hTouchInput,
    UINT cInputs,
    PTOUCHINPUT pInputs,
    int cbSize
)
{
    bool result = false;
    static bool mouse_state_old = false;
    for (UINT input = 0; input < cInputs; input++) {
        TOUCHINPUT *touch_input = &pInputs[input];

        touch_input->x = 0;
        touch_input->y = 0;
        touch_input->hSource = NULL;
        touch_input->dwID = 0;
        touch_input->dwFlags = 0;
        touch_input->dwMask = 0;
        touch_input->dwTime = 0;
        touch_input->dwExtraInfo = 0;
        touch_input->cxContact = 0;
        touch_input->cyContact = 0;

        bool mouse_state = (GetKeyState(VK_LBUTTON) & 0x100) != 0;

        if (mouse_state || mouse_state_old) {
            POINT cursorPos;
            GetCursorPos(&cursorPos);

            result = true;
            touch_input->x = cursorPos.x * 100;
            touch_input->y = cursorPos.y * 100;
            touch_input->hSource = hTouchInput;
            touch_input->dwID = 0;
            touch_input->dwFlags = 0;
            if (mouse_state && !mouse_state_old) {
                touch_input->dwFlags |= TOUCHEVENTF_DOWN;
            } else if (mouse_state && mouse_state_old) {
                touch_input->dwFlags |= TOUCHEVENTF_MOVE;
            } else if (!mouse_state && mouse_state_old) {
                touch_input->dwFlags |= TOUCHEVENTF_UP;
            }
            touch_input->dwMask = 0;
            touch_input->dwTime = 0;
            touch_input->dwExtraInfo = 0;
            touch_input->cxContact = 0;
            touch_input->cyContact = 0;
        }
        mouse_state_old = mouse_state;
    }

    return result;
}