cmake_minimum_required(VERSION 3.16)

project(MY3D)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20") #-fsanitize=address

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

include_directories(${PROJECT_SOURCE_DIR}/opengl)
include_directories(${PROJECT_SOURCE_DIR}/opengl/glad/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/opengl/glad/src GLAD_SRC)
include_directories(${PROJECT_SOURCE_DIR}/utility/model)
include_directories(${PROJECT_SOURCE_DIR}/utility/animation)
include_directories(${PROJECT_SOURCE_DIR}/utility/physics)
include_directories(${PROJECT_SOURCE_DIR}/utility/mylog/inc)
aux_source_directory(${PROJECT_SOURCE_DIR}/utility/mylog/src LOG_SRC)

add_executable(my3D ${PROJECT_SOURCE_DIR}/sample/main.cpp ${GLAD_SRC} ${LOG_SRC})
find_package(OpenGL REQUIRED)
find_package(glfw3 3.4 REQUIRED)
find_package(assimp REQUIRED)
target_link_libraries(my3D
    OpenGL::GL
    glfw
    assimp::assimp
    stb
)
