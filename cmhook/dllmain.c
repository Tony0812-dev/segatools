#include <windows.h>

#include <stdlib.h>

#include "board/io4.h"
#include "board/sg-reader.h"
#include "board/vfd.h"

#include "hook/process.h"

#include "hooklib/dvd.h"
#include "hooklib/touch.h"
#include "hooklib/serial.h"
#include "hooklib/spike.h"

#include "cmhook/config.h"
#include "cmhook/io4.h"
#include "cmhook/cm-dll.h"
#include "cmhook/unity.h"

#include "platform/platform.h"

#include "util/dprintf.h"

static HMODULE cm_hook_mod;
static process_entry_t cm_startup;
static struct cm_hook_config cm_hook_cfg;

static DWORD CALLBACK cm_pre_startup(void)
{
    HRESULT hr;

    dprintf("--- Begin cm_pre_startup ---\n");

    /* Load config */

    cm_hook_config_load(&cm_hook_cfg, L".\\segatools.ini");

    /* Hook Win32 APIs */

    dvd_hook_init(&cm_hook_cfg.dvd, cm_hook_mod);
    touch_hook_init(&cm_hook_cfg.touch, cm_hook_mod);
    serial_hook_init();

    /* Initialize emulation hooks */

    hr = platform_hook_init(
            &cm_hook_cfg.platform,
            "SDED",
            "ACA1",
            cm_hook_mod);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = sg_reader_hook_init(&cm_hook_cfg.aime, 1, cm_hook_mod);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = vfd_hook_init(3);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = cm_dll_init(&cm_hook_cfg.dll, cm_hook_mod);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = cm_io4_hook_init(&cm_hook_cfg.io4);

    if (FAILED(hr)) {
        goto fail;
    }

    /* Initialize Unity native plugin DLL hooks

       There seems to be an issue with other DLL hooks if `LoadLibraryW` is
       hooked earlier in the `cmhook` initialization. */

    unity_hook_init();

    /* Initialize debug helpers */

    spike_hook_init(L".\\segatools.ini");

    dprintf("---  End  cm_pre_startup ---\n");

    /* Jump to EXE start address */

    return cm_startup();

fail:
    ExitProcess(EXIT_FAILURE);
}

BOOL WINAPI DllMain(HMODULE mod, DWORD cause, void *ctx)
{
    HRESULT hr;

    if (cause != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    cm_hook_mod = mod;

    hr = process_hijack_startup(cm_pre_startup, &cm_startup);

    if (!SUCCEEDED(hr)) {
        dprintf("Failed to hijack process startup: %x\n", (int) hr);
    }

    return SUCCEEDED(hr);
}
