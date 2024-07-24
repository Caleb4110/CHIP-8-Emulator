set(BINARY ${CMAKE_PROJECT_NAME}_test)

list(APPEND MY_SOURCES
        chip8.test.cpp
)

add_executable(${BINARY} ${MY_SOURCES})

target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest_main ${SDL2_LIBRARIES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
