cmake_minimum_required(VERSION 3.5)
project(lesson1 C)

set(LIBRARIES "SDL2")

add_executable(${PROJECT_NAME} lesson1.c)
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
