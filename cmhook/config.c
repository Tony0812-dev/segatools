#include <assert.h>
#include <stddef.h>

#include "board/config.h"

#include "hooklib/config.h"
#include "hooklib/dvd.h"
#include "hooklib/touch.h"

#include "cmhook/config.h"

#include "platform/config.h"

void cm_dll_config_load(
        struct cm_dll_config *cfg,
        const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    GetPrivateProfileStringW(
            L"cmio",
            L"path",
            L"",
            cfg->path,
            _countof(cfg->path),
            filename);
}

void cm_hook_config_load(
        struct cm_hook_config *cfg,
        const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    platform_config_load(&cfg->platform, filename);
    aime_config_load(&cfg->aime, filename);
    dvd_config_load(&cfg->dvd, filename);
    io4_config_load(&cfg->io4, filename);
    touch_config_load(&cfg->touch, filename);
    cm_dll_config_load(&cfg->dll, filename);
}
