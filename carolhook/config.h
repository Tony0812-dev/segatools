#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "amex/amex.h"

#include "board/sg-reader.h"

#include "carolhook/carol-dll.h"
#include "carolhook/controlbd.h"

#include "hooklib/gfx.h"
#include "hooklib/touch.h"
#include "hooklib/dvd.h"

#include "platform/platform.h"

struct carol_hook_config {
    struct platform_config platform;
    struct amex_config amex;
    struct aime_config aime;
    struct gfx_config gfx;
    struct dvd_config dvd;
    struct touch_config touch;
    struct controlbd_config controlbd;
    struct carol_dll_config dll;
};

void carol_dll_config_load(
        struct carol_dll_config *cfg,
        const wchar_t *filename);
void controlbd_config_load(struct controlbd_config *cfg, const wchar_t *filename);
void carol_hook_config_load(
        struct carol_hook_config *cfg,
        const wchar_t *filename);
