#include <assert.h>
#include <stddef.h>

#include "board/config.h"

#include "hooklib/config.h"
#include "hooklib/dvd.h"
#include "hooklib/touch.h"
#include "hooklib/gfx.h"

#include "fgohook/config.h"

#include "platform/config.h"

void fgo_dll_config_load(
        struct fgo_dll_config *cfg,
        const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    GetPrivateProfileStringW(
            L"fgoio",
            L"path",
            L"",
            cfg->path,
            _countof(cfg->path),
            filename);
}

void fgo_hook_config_load(
        struct fgo_hook_config *cfg,
        const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    platform_config_load(&cfg->platform, filename);
    aime_config_load(&cfg->aime, filename);
    dvd_config_load(&cfg->dvd, filename);
    touch_config_load(&cfg->touch, filename);
    fgo_dll_config_load(&cfg->dll, filename);
}
