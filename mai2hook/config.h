#pragma once

#include <stddef.h>

#include "board/config.h"

#include "hooklib/dvd.h"
#include "hooklib/gfx.h"

#include "platform/config.h"

struct mai2_hook_config {
    struct platform_config platform;
    struct aime_config aime;
    struct dvd_config dvd;
    struct gfx_config gfx;
};

void mai2_hook_config_load(
        struct mai2_hook_config *cfg,
        const wchar_t *filename);
