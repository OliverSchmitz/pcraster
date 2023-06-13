set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# On Linux/macOS we set the RPATH for our libraries/executables
# From the CMake wiki:

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  OFF)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)

# the RPATH to be used when installing
set(CMAKE_INSTALL_RPATH "\\\$ORIGIN/../lib")

# don't add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setting postfix is disabled in several Python modules
# and linkin related libraries
set(CMAKE_DEBUG_POSTFIX "_d")

# When not specified: default build type set to release for single-configuration generators
get_property(is_multiconfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(NOT is_multiconfig)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif()
endif()


if(PCRASTER_WITH_FLAGS_NATIVE)
    add_compile_options(
        "$<$<CONFIG:Release>:$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang,Clang>>:-march=native;-mtune=native>>"
    )
    add_compile_options(
        "$<$<CONFIG:RelWithDebInfo>:$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang,Clang>>:-march=native;-mtune=native>>"
    )
endif()

add_compile_options(
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang,Clang>>:-pipe>"
    "$<$<COMPILE_LANG_AND_ID:C,GNU,AppleClang,Clang>:>"
    "$<$<COMPILE_LANG_AND_ID:CXX,GNU,AppleClang,Clang>:>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W1>"
    "$<$<AND:$<PLATFORM_ID:Linux>,$<COMPILE_LANG_AND_ID:C,GNU>,$<CONFIG:Debug>>:-Wall;-pedantic;-Wpointer-arith;-Wdeclaration-after-statement>"
    "$<$<AND:$<PLATFORM_ID:Linux>,$<COMPILE_LANG_AND_ID:CXX,GNU>,$<CONFIG:Debug>>:>"
    "$<$<AND:$<PLATFORM_ID:Linux>,$<COMPILE_LANG_AND_ID:C,GNU>,$<CONFIG:Release>>:-pedantic;-Wpointer-arith;-Wdeclaration-after-statement>"
    "$<$<AND:$<PLATFORM_ID:Linux>,$<COMPILE_LANG_AND_ID:CXX,GNU>,$<CONFIG:Release>>:>"
    #"$<$<AND:$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:Clang>>:-stdlib=libc++>" #;-lc++abi  -stdlib=libc++;-D_GLIBCXX_USE_CXX11_ABI=0
)

add_compile_definitions(
  "$<$<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>:_CRT_SECURE_NO_WARNINGS;_USE_MATH_DEFINES;NOMINMAX>"
)

# Based on conda and https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc/
# https://wiki.ubuntu.com/ToolChain/CompilerFlags?action=show&redirect=CompilerFlags
# https://wiki.debian.org/Hardening
add_link_options(
    "$<$<OR:$<CXX_COMPILER_ID:GNU>>:-Wl,--no-undefined;-Wl,--as-needed;-Wl,--sort-common;-Wl,--gc-sections;-Wl,-O2;-Wl,-z,now;-Wl,-z,relro;-Wl,-z,defs;-Wl,--warn-common;-Wl,--hash-style=gnu;-Wl,--no-copy-dt-needed-entries>"
    "$<$<AND:$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:Clang>>:-Wl,--as-needed;-Wl,--sort-common;-Wl,--gc-sections;-Wl,-O2;-Wl,-z,now;-Wl,-z,relro;-Wl,--warn-common;-Wl,--hash-style=gnu;-Wl,--no-copy-dt-needed-entries>"

)

if(PCRASTER_WITH_FLAGS_IPO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT COMPILER_HAS_IPO)
    # This takes long for the unit tests...
    if(COMPILER_HAS_IPO)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()



# In general, don't set compiler options in CMake files. Here we set the
# most general options that everybody always wants. Anything else should
# be handled from the outside.
# Not yet (treat warnings as errors):
#   MSVC: /WX
#   GNU/Clang: -Werror
# add_compile_options(
#     "$<$<CXX_COMPILER_ID:MSVC>:/W3 /WX>"
#     "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-W -Wall>"
# )



# TODO Get rid of these...
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DDEBUG_BUILD -DDEBUG_DEVELOP"
)
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -DDEBUG_BUILD -DDEBUG_DEVELOP"
)

# if(MSVC)
#     # TODO add debug/release flags?
#
#     # Get rid of the min and max macros.
#     # Refactor the define private/protected public stuff (allow keywords macro)
#     add_compile_definitions(
#         -D_SCL_SECURE_NO_WARNINGS
#         -D_CRT_SECURE_NO_WARNINGS
#         -D_USE_MATH_DEFINES
#         -DNOMINMAX
#         -D_ALLOW_KEYWORD_MACROS
#     )
#
#
#     # add /w3
#     # disable these warnings?
#     set(CMAKE_CXX_FLAGS
#         "${CMAKE_CXX_FLAGS} /std:c++14 /wd4244 /wd4396 /wd4305"
#     )
#
# endif()
