#pragma once

#include <windows.h>

#include <stdbool.h>

struct controlbd_config {
    bool enable;
};

HRESULT controlbd_hook_init(const struct controlbd_config *cfg);
