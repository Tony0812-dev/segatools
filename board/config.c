#include <windows.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "board/aime-dll.h"
#include "board/config.h"
#include "board/sg-reader.h"

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

static void aime_dll_config_load(struct aime_dll_config *cfg, const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);
    
    // Workaround for x64/x86 external aime IO dlls
    // path32 for 32bit, path64 for 64bit

    #if defined(ENV32BIT)
        GetPrivateProfileStringW(
                L"aimeio",
                L"path32",
                L"",
                cfg->path,
                _countof(cfg->path),
                filename);
    #elif defined(ENV64BIT)
        GetPrivateProfileStringW(
                L"aimeio",
                L"path64",
                L"",
                cfg->path,
                _countof(cfg->path),
                filename);
    #else
        #error "Unknown environment"
    #endif

}

void aime_config_load(struct aime_config *cfg, const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    aime_dll_config_load(&cfg->dll, filename);
    cfg->enable = GetPrivateProfileIntW(L"aime", L"enable", 1, filename);
    cfg->high_baudrate = GetPrivateProfileIntW(L"aime", L"highbaud", 1, filename);
}

void io4_config_load(struct io4_config *cfg, const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    cfg->enable = GetPrivateProfileIntW(L"io4", L"enable", 1, filename);
}
