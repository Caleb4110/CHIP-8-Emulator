set(BINARY ${CMAKE_PROJECT_NAME})

list(APPEND MY_SOURCES
        chip8.cpp
        chip8.h
        window.cpp
        window.h
        main.cpp
)

add_executable(${BINARY} ${MY_SOURCES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

add_library(${BINARY}_lib STATIC ${MY_SOURCES})
