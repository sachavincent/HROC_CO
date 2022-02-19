set(BINARY ${CMAKE_PROJECT_NAME}_TESTS)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(ASSIMP_BUILD_SAMPLES OFF)
set(ASSIMP_BUILD_ZLIB ON)
set(BUILD_ASSIMP_TOOLS  OFF)
set(ASSIMP_BUILD_DOCS OFF)
set(ASSIMP_BUILD_STATIC_LIB ON)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL_PDB OFF)
set(BUILD_SHARED_LIBS OFF)
           
include(${PROJECT_SOURCE_DIR}/external/DisableAssimpImports.cmake)
disableAssimpImports()

include_directories(${PROJECT_SOURCE_DIR}/external/assimp)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(BUILD_SHARED_LIBS OFF)
include_directories(${PROJECT_SOURCE_DIR}/external/glfw)

SET(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

file(GLOB TEST_SOURCES  src/*.cpp)

file(GLOB APP_HEADERS   ${PROJECT_SOURCE_DIR}/include/frustum.hpp)
file(GLOB APP_SOURCES   ${PROJECT_SOURCE_DIR}/src/bvh/bvhtree.cpp
                        ${PROJECT_SOURCE_DIR}/src/bvh/bvhnode.cpp
                        ${PROJECT_SOURCE_DIR}/src/bvh/boundingbox.cpp
                        ${PROJECT_SOURCE_DIR}/src/camera.cpp
                        ${PROJECT_SOURCE_DIR}/src/model/*.cpp
                        ${PROJECT_SOURCE_DIR}/src/texture.cpp
                        ${PROJECT_SOURCE_DIR}/src/shader.cpp
                        ${PROJECT_SOURCE_DIR}/external/glad/src/glad.c
                        #etc
)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /DGLFW_INCLUDE_NONE /std:c++17 /MP8 /wd4100 /wd4505 /wd4201 /wd4305 /wd4267 /wd4244 /wd4996 /wd4018")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare -Wno-unused-variable -Wno-reorder -Wno-return-local-addr -Wno-format-security -std=c++17 -DNOT_MSVC")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/external/glm/
                    ${PROJECT_SOURCE_DIR}/external/stb/
                    ${PROJECT_SOURCE_DIR}/external/glad/include/
                    ${PROJECT_SOURCE_DIR}/external/glfw/include/
                    ${PROJECT_SOURCE_DIR}/external/assimp/include/
)

add_executable(${BINARY} ${APP_SOURCES} ${TEST_SOURCES})

add_test(NAME ${BINARY} COMMAND ${BINARY} "src/main.cpp")

target_link_libraries(${BINARY} PUBLIC gtest glfw assimp ${GLFW_LIBRARIES} ${GLAD_LIBRARIES})

if(MSVC)
target_compile_options( ${BINARY}    PUBLIC "/DHROC_TESTS" )
else()
target_compile_options( ${BINARY}    PUBLIC "-DHROC_TESTS" )
endif()