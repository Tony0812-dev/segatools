#pragma once

#include <stddef.h>

#include "board/config.h"

#include "hooklib/touch.h"
#include "hooklib/dvd.h"

#include "fgohook/fgo-dll.h"

#include "platform/config.h"

struct fgo_hook_config {
    struct platform_config platform;
    struct aime_config aime;
    struct dvd_config dvd;
    struct io4_config io4;
    struct touch_config touch;
    struct fgo_dll_config dll;
};

void fgo_dll_config_load(
        struct fgo_dll_config *cfg,
        const wchar_t *filename);

void fgo_hook_config_load(
        struct fgo_hook_config *cfg,
        const wchar_t *filename);
