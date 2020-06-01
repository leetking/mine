cmake_minimum_required(VERSION 3.5)

project(mine LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(app mine)
set(ver 0.0.2)
#set(DEBUG False)
set(third_libs sdl2 SDL2_image SDL2_ttf)
#set(CMAKE_BUILD_TYPE DEBUG/RELEASE)
#set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
#set(cmake_C_FLAGS_DEBUG "-g0")
#set(cmake_CXX_FLAGS_RELEASE "")
#set(cmake_C_FLAGS_RELEASE "")

find_package(PkgConfig REQUIRED)
pkg_check_modules(THIRD_LIBS REQUIRED ${third_libs})

#set(CMAKE_C_FLAGS -DVERSION=\"v${ver}\")

# TODO 自动扫描源码文件
add_executable(${app}
    src/main.c
    src/model.c
    src/timer.c
    src/model.h
    src/timer.h
)

target_compile_definitions(${app} PRIVATE VERSION=\"v${ver}\")
target_include_directories(${app} PUBLIC ${THIRD_LIBS_INCLUDE_DIRS})
target_compile_options(${app} PUBLIC ${THIRD_LIBS_CFLAGS_OTHER})
target_link_libraries(${app} PRIVATE ${THIRD_LIBS_LIBRARIES})

# auto copy dlls if on windows
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")   # depress cammand line window
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND windeployqt $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ldd ${APP} | grep '/mingw64' | awk '{print $$3}' | xargs -I dll cp dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
endif (WIN32)

# copy compile_commands.json to sources directory
if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
endif ()
