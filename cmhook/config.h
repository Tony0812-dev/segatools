#pragma once

#include <stddef.h>

#include "board/config.h"

#include "hooklib/dvd.h"

#include "cmhook/cm-dll.h"

#include "platform/config.h"

struct cm_hook_config {
    struct platform_config platform;
    struct aime_config aime;
    struct dvd_config dvd;
    struct io4_config io4;
    struct cm_dll_config dll;
    struct touch_config touch;
};

void cm_dll_config_load(
        struct cm_dll_config *cfg,
        const wchar_t *filename);

void cm_hook_config_load(
        struct cm_hook_config *cfg,
        const wchar_t *filename);
