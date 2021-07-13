#include <windows.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include "amex/amex.h"
#include "amex/config.h"

#include "board/config.h"
#include "board/sg-reader.h"

#include "carolhook/config.h"

#include "hooklib/config.h"
#include "hooklib/gfx.h"

#include "platform/config.h"
#include "platform/platform.h"

void carol_dll_config_load(
        struct carol_dll_config *cfg,
        const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    GetPrivateProfileStringW(
            L"carolio",
            L"path",
            L"",
            cfg->path,
            _countof(cfg->path),
            filename);
}

void controlbd_config_load(struct controlbd_config *cfg, const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    cfg->enable = GetPrivateProfileIntW(L"controlbd", L"enable", 1, filename);
}

void carol_hook_config_load(
        struct carol_hook_config *cfg,
        const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    memset(cfg, 0, sizeof(*cfg));

    platform_config_load(&cfg->platform, filename);
    amex_config_load(&cfg->amex, filename);
    aime_config_load(&cfg->aime, filename);
    gfx_config_load(&cfg->gfx, filename);
    carol_dll_config_load(&cfg->dll, filename);
    controlbd_config_load(&cfg->controlbd, filename);
}
