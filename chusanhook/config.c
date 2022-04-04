#include <assert.h>
#include <stddef.h>

#include "board/config.h"

#include "hooklib/config.h"
#include "hooklib/dvd.h"
#include "hooklib/gfx.h"

#include "platform/config.h"

#include "chusanhook/config.h"

// Check windows
#if _WIN32 || _WIN64
   #if _WIN64
     #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif

// Check GCC
#if __GNUC__
  #if __x86_64__ || __ppc64__
    #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif

void chuni_dll_config_load(
        struct chuni_dll_config *cfg,
        const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    // Workaround for x64/x86 external IO dlls
    // path32 for 32bit, path64 for 64bit
    // for else.. is that possible? idk

    #if defined(ENV32BIT)
        GetPrivateProfileStringW(
                L"chuniio",
                L"path32",
                L"",
                cfg->path,
                _countof(cfg->path),
                filename);
    #elif defined(ENV64BIT)
        GetPrivateProfileStringW(
                L"chuniio",
                L"path64",
                L"",
                cfg->path,
                _countof(cfg->path),
                filename);
    #else
        #error "Unknown environment"
    #endif
    
}

void slider_config_load(struct slider_config *cfg, const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    cfg->enable = GetPrivateProfileIntW(L"slider", L"enable", 1, filename);
}

void chusan_hook_config_load(
        struct chusan_hook_config *cfg,
        const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    memset(cfg, 0, sizeof(*cfg));

    platform_config_load(&cfg->platform, filename);
    aime_config_load(&cfg->aime, filename);
    dvd_config_load(&cfg->dvd, filename);
    io4_config_load(&cfg->io4, filename);
    gfx_config_load(&cfg->gfx, filename);
    chuni_dll_config_load(&cfg->dll, filename);
    slider_config_load(&cfg->slider, filename);
}
