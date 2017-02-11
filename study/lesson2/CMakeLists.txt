cmake_minimum_required (VERSION 3.5)
project(lesson2 C)

add_executable(${PROJECT_NAME} lesson2.c)
target_link_libraries(${PROJECT_NAME} "SDL2")

# what is CMAKE_SOURCE_DIR ?
file(GLOB RES_FILES "${CMAKE_SOURCE_DIR}/imgs/*.*")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/imgs/)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RES_FILES} $<TARGET_FILE_DIR:${PROJECT_NAME}>/imgs/)
