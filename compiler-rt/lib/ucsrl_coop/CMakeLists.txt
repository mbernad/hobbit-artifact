add_compiler_rt_component(coop_hash)

set(UCSRL_COOP_HASH_CFLAGS)

list(APPEND UCSRL_COOP_HASH_CFLAGS
        -O3)

set(UCSRL_COOP_HASH_SOURCES
        coop_hash.cpp
        )


include_directories(..)

foreach(arch ${ALL_UCSRL_COOP_SUPPORTED_ARCH})
    message(STATUS "Building UCSRL_COOP coop_hash RT library for ${arch}")
    add_compiler_rt_runtime(clang_rt.coop_hash
            STATIC
            ARCHS ${arch}
            SOURCES ${UCSRL_COOP_HASH_SOURCES}
            CFLAGS ${UCSRL_COOP_HASH_CFLAGS}
            PARENT_TARGET coop_hash)
endforeach()