cmake_minimum_required(VERSION 3.2)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project(HROC_APP)

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
set(JSON_BuildTests OFF CACHE INTERNAL "")

include(external/DisableAssimpImports.cmake)
disableAssimpImports()

add_subdirectory(external/assimp)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(external/glfw)
#set(gtest_force_shared_crt 1 CACHE BOOL "")
add_subdirectory(external/googletest)
add_subdirectory(external/nlohmann_json)
add_subdirectory(test)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /DGLFW_INCLUDE_NONE /std:c++17 /MP8 
        /wd4100 /wd4505 /wd4201 /wd4305 /wd4267 /wd4244 /wd4996 /wd4018 /DUSE_BOOKMARK")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare  -Wno-unused-variable -Wno-reorder -Wno-return-local-addr -Wno-format-security -std=c++17 -ltbb -DNOT_MSVC -DUSE_BOOKMARK ")
endif()

include_directories(include/
                    external/glad/include/
                    external/glfw/include/
                    external/glm/
                    external/stb/
					external/zlib/
                    external/imgui/
                    external/imgui/backends/
                    external/assimp/include/
                    external/implot/
                    external/ImGuiFileDialog/
                    external/ImGuiFileDialog/*.h
					)

file(GLOB VENDORS_SOURCES external/glad/src/glad.c)

file(GLOB PROJECT_HEADERS include/*.hpp )
file(GLOB PROJECT_SOURCES src/*.cpp
                          src/model/*.cpp
                          src/bvh/*.cpp
                          external/imgui/*.cpp
                          external/imgui/backends/*opengl3.cpp
                          external/imgui/backends/*glfw.cpp
                          external/implot/implot_items.cpp
                          external/implot/implot.cpp
                          external/ImGuiFileDialog/ImGuiFileDialog.cpp
                        )

file(GLOB PROJECT_SHADERS shaders/*.frag
                          shaders/*.vert)

file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitignore
                         .gitmodules)                         

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES}  "src/main.cpp")

target_link_libraries(${PROJECT_NAME} glfw assimp
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} nlohmann_json::nlohmann_json)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

message("### Moving assets next to binary dir ####")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_SHADERS})