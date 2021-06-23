shared_library(
    'mai2hook',
    name_prefix : '',
    include_directories : inc,
    implicit_include_directories : false,
    vs_module_defs : 'mai2hook.def',
    c_pch : '../precompiled.h',
    dependencies : [
        capnhook.get_variable('hook_dep'),
        capnhook.get_variable('hooklib_dep'),
    ],
    link_with : [
        aimeio_lib,
        board_lib,
        hooklib_lib,
        platform_lib,
        util_lib,
    ],
    sources : [
        'config.c',
        'config.h',
        'dllmain.c',
        'io4.c',
        'io4.h',
        'unity.h',
        'unity.c',
    ],
)
