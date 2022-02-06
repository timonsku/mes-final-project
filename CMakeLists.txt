cmake_minimum_required(VERSION 3.14)
include(FetchContent)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

include(example_auto_set_url.cmake)
include_directories(${CMAKE_SOURCE_DIR}/lib)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

#add_subdirectory(sharp_display)

if (TARGET tinyusb_device)
add_executable( display_test
        display_test.c SHARP_MIP.c
        )

set(LV_CONF_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/lv_conf.h
    CACHE STRING "" FORCE)

# target_include_directories(lvgl PRIVATE ${lib/lvgl})

FetchContent_Declare(lvgl
    GIT_REPOSITORY https://github.com/lvgl/lvgl.git
    GIT_TAG v8.2.0)
FetchContent_MakeAvailable(lvgl)

set_target_properties(lvgl_demos lvgl_examples PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

# pull in common dependencies and additional spi hardware support
target_link_libraries(display_test PRIVATE pico_stdlib pico_malloc hardware_spi lvgl::lvgl)

# enable usb output, disable uart output
pico_enable_stdio_usb(display_test 1)
pico_enable_stdio_uart(display_test 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(display_test)

# add url via pico_set_program_url
example_auto_set_url(display_test)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()
