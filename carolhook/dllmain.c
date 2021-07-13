#include <windows.h>

#include <stdlib.h>

#include "amex/amex.h"

#include "board/sg-reader.h"

#include "carolhook/config.h"
#include "carolhook/jvs.h"
#include "carolhook/controlbd.h"

#include "carolio/carolio.h"

#include "hook/process.h"

#include "hooklib/gfx.h"
#include "hooklib/serial.h"
#include "hooklib/spike.h"
#include "hooklib/dvd.h"
#include "hooklib/touch.h"

#include "platform/platform.h"

#include "util/dprintf.h"

static HMODULE carol_hook_mod;
static process_entry_t carol_startup;
static struct carol_hook_config carol_hook_cfg;

static DWORD CALLBACK carol_pre_startup(void)
{
    HMODULE d3dc;
    HMODULE dbghelp;
    HRESULT hr;

    dprintf("--- Begin carol_pre_startup ---\n");

    /* Pin the D3D shader compiler. This makes startup much faster. */

    d3dc = LoadLibraryW(L"D3DCompiler_43.dll");

    if (d3dc != NULL) {
        dprintf("Pinned shader compiler, hMod=%p\n", d3dc);
    } else {
        dprintf("Failed to load shader compiler!\n");
    }

    /* Pin dbghelp so the path hooks apply to it. */

    dbghelp = LoadLibraryW(L"dbghelp.dll");

    if (dbghelp != NULL) {
        dprintf("Pinned debug helper library, hMod=%p\n", dbghelp);
    } else {
        dprintf("Failed to load debug helper library!\n");
    }

    /* Config load */

    carol_hook_config_load(&carol_hook_cfg, L".\\segatools.ini");

    /* Hook Win32 APIs */

    gfx_hook_init(&carol_hook_cfg.gfx, carol_hook_mod);
    dvd_hook_init(&carol_hook_cfg.dvd, carol_hook_mod);
    touch_hook_init(&carol_hook_cfg.touch, carol_hook_mod);
    serial_hook_init();

    /* Initialize emulation hooks */

    hr = platform_hook_init(
            &carol_hook_cfg.platform,
            "SDAP",
            "AAV0",
            carol_hook_mod);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = carol_dll_init(&carol_hook_cfg.dll, carol_hook_mod);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = amex_hook_init(&carol_hook_cfg.amex, carol_jvs_init);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = controlbd_hook_init(&carol_hook_cfg.controlbd);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = sg_reader_hook_init(&carol_hook_cfg.aime, 12, carol_hook_mod);

    if (FAILED(hr)) {
        goto fail;
    }

    /* Initialize debug helpers */

    spike_hook_init(L".\\segatools.ini");

    dprintf("---  End  carol_pre_startup ---\n");

    /* Jump to EXE start address */

    return carol_startup();

fail:
    ExitProcess(EXIT_FAILURE);
}

BOOL WINAPI DllMain(HMODULE mod, DWORD cause, void *ctx)
{
    HRESULT hr;

    if (cause != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    carol_hook_mod = mod;

    hr = process_hijack_startup(carol_pre_startup, &carol_startup);

    if (!SUCCEEDED(hr)) {
        dprintf("Failed to hijack process startup: %x\n", (int) hr);
    }

    return SUCCEEDED(hr);
}
