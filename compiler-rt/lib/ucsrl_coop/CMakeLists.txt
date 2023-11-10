add_compiler_rt_component(coop_hash)

set(UCSRL_COOP_HASH_CFLAGS)

list(APPEND UCSRL_COOP_HASH_CFLAGS
        -O3)

set(UCSRL_COOP_HASH_SOURCES
        coop_hash.cpp
        )


include_directories(..)

UNSET(libhighway_location CACHE)
find_library(libhighway_location highwayhash PATHS lib NO_DEFAULT_PATH)
message(STATUS ${libhighway_location})

UNSET(libhighway CACHE)
add_library(libhighway SHARED IMPORTED)
set_target_properties(libhighway PROPERTIES IMPORTED_LOCATION ${libhighway_location})

foreach(arch ${ALL_UCSRL_COOP_SUPPORTED_ARCH})
    message(STATUS "Building UCSRL_COOP coop_hash RT library for ${arch}")
    UNSET(clang_rt.coop_hash CACHE)
    add_compiler_rt_runtime(clang_rt.coop_hash
            SHARED
            ARCHS ${arch}
            LINK_LIBS libhighway
            SOURCES ${UCSRL_COOP_HASH_SOURCES}
            CFLAGS ${UCSRL_COOP_HASH_CFLAGS}
            PARENT_TARGET coop_hash)
endforeach()