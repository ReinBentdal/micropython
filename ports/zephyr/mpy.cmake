
set(MICROPY_PORT_DIR    ${CMAKE_CURRENT_SOURCE_DIR})
set(MICROPY_DIR         ${MICROPY_PORT_DIR}/../..)
set(MICROPY_TARGET      micropython)

include(${MICROPY_DIR}/py/py.cmake)
include(${MICROPY_DIR}/extmod/extmod.cmake)

set(MICROPY_SOURCE_PORT
    main.c
    mphalport.c
)
list(TRANSFORM MICROPY_SOURCE_PORT PREPEND ${MICROPY_PORT_DIR}/)

set(MICROPY_SOURCE_SHARED
    libc/printf.c
    runtime/stdout_helpers.c
)
list(TRANSFORM MICROPY_SOURCE_SHARED PREPEND ${MICROPY_DIR}/shared/)

set(MICROPY_SOURCE_LIB)
list(TRANSFORM MICROPY_SOURCE_LIB PREPEND ${MICROPY_DIR}/lib/)

set(MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_SHARED}
    ${MICROPY_SOURCE_LIB}
    ${MICROPY_SOURCE_PORT}
)

zephyr_get_include_directories_for_lang(C includes)
zephyr_get_system_include_directories_for_lang(C system_includes)
zephyr_get_compile_definitions_for_lang(C definitions)
zephyr_get_compile_options_for_lang(C options)

set(MICROPY_CPP_FLAGS_EXTRA ${includes} ${system_includes} ${definitions} ${options})

zephyr_library_named(${MICROPY_TARGET})

zephyr_library_include_directories(
    ${MICROPY_INC_CORE}
    ${MICROPY_PORT_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

zephyr_library_compile_options(
    -std=gnu99 -fomit-frame-pointer
)

zephyr_library_compile_definitions(
    NDEBUG
    MP_CONFIGFILE=<${CONFIG_MICROPY_CONFIGFILE}>
    MICROPY_HEAP_SIZE=${CONFIG_MICROPY_HEAP_SIZE}
)

zephyr_library_sources(${MICROPY_SOURCE_QSTR})

add_dependencies(${MICROPY_TARGET} zephyr_generated_headers)

include(${MICROPY_DIR}/py/mkrules.cmake)

include_directories(
    ${MICROPY_INC_CORE}
    ${MICROPY_PORT_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)