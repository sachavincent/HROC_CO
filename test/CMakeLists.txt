set(BINARY ${CMAKE_PROJECT_NAME}_TESTS)

SET(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

file(GLOB TEST_SOURCES  src/*.cpp)
file(GLOB APP_SOURCES   ${PROJECT_SOURCE_DIR}/src/bvhtree.cpp
                        ${PROJECT_SOURCE_DIR}/src/bvhnode.cpp)
set(SOURCES ${TEST_SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/external/glm/)

add_executable(${BINARY} ${APP_SOURCES} ${TEST_SOURCES})

add_test(NAME ${BINARY} COMMAND ${BINARY} "src/bvhTest.cpp")

target_link_libraries(${BINARY} PUBLIC gtest)

