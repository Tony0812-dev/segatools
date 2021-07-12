#pragma once

#include <windows.h>

#include <stdbool.h>

struct touch_config {
    bool enable;
};

/* Init is not thread safe because API hook init is not thread safe blah
    blah blah you know the drill by now. */

void touch_hook_init(const struct touch_config *cfg, HINSTANCE self);