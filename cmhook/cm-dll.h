#pragma once

#include <windows.h>

#include "cmio/cmio.h"

struct cm_dll {
    uint16_t api_version;
    HRESULT (*init)(void);
    HRESULT (*poll)(void);
    void (*get_opbtns)(uint8_t *opbtn);
};

struct cm_dll_config {
    wchar_t path[MAX_PATH];
};

extern struct cm_dll cm_dll;

HRESULT cm_dll_init(const struct cm_dll_config *cfg, HINSTANCE self);
