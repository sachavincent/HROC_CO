set(BINARY ${CMAKE_PROJECT_NAME}_TESTS)

SET(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

file(GLOB TEST_SOURCES src/*.cpp)

set(SOURCES ${TEST_SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(${BINARY} ${TEST_SOURCES})

add_test(NAME ${BINARY} COMMAND ${BINARY} "src/main.cpp")

target_link_libraries(${BINARY} PUBLIC gtest)

