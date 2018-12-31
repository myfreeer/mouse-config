cmake_minimum_required(VERSION 3.13)
project(mouse_config C)

set(CMAKE_C_STANDARD 11)

add_executable(main main.c base.h wmi.h mouse.h ini.h)
target_link_libraries(main ole32 wbemuuid)
add_executable(daemon daemon.c base.h wmi.h mouse.h ini.h daemon.h timer.h)
target_link_libraries(daemon ole32 wbemuuid)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS "-pipe -Wall -Wextra -fmerge-all-constants -Wl,--gc-sections,--build-id=none")
    set(CMAKE_C_FLAGS_DEBUG "-g -Og")
    set(CMAKE_C_FLAGS_RELEASE "-s -Os")
    if (WIN32)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles -DNO_START_FILES")
        set_target_properties(daemon PROPERTIES
                COMPILE_FLAGS "${CMAKE_C_FLAGS} -mwindows"
                LINK_FLAGS "${CMAKE_C_FLAGS} -mwindows")
    endif ()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_C_FLAGS}")
endif ()