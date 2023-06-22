cmake_minimum_required(VERSION 3.5)

add_library(GUI STATIC
    src/Window.cpp
    src/KeyCallback.cpp
    src/SizeCallback.cpp
    src/Update.cpp
    src/Draw.cpp
    src/Shader.cpp
    src/Text.cpp
)

target_include_directories(GUI PUBLIC include)

# GLFWのリンク
target_link_libraries(GUI PUBLIC GL glfw GLEW)

# FreeType
find_package(Freetype REQUIRED)
target_include_directories(GUI PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(GUI PRIVATE ${FREETYPE_LIBRARIES})
