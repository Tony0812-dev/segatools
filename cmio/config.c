#include <windows.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "cmio/config.h"

void cm_io_config_load(
        struct cm_io_config *cfg,
        const wchar_t *filename)
{
    wchar_t key[16];
    int i;

    assert(cfg != NULL);
    assert(filename != NULL);

    cfg->vk_test = GetPrivateProfileIntW(L"io4", L"test", '1', filename);
    cfg->vk_service = GetPrivateProfileIntW(L"io4", L"service", '2', filename);
    cfg->vk_coin = GetPrivateProfileIntW(L"io4", L"coin", '3', filename);
}
