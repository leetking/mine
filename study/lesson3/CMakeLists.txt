cmake_minimum_required(VERSION 3.5)
project(lesson3 C)

add_executable(${PROJECT_NAME} lesson3.c Timer.c)
target_link_libraries(${PROJECT_NAME} "SDL2" "SDL2_image")

file(GLOB IMGS "${CMAKE_SOURCE_DIR}/imgs/*.*")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/imgs/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${IMGS} "$<TARGET_FILE_DIR:${PROJECT_NAME}>/imgs/")
