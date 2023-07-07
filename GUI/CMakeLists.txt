cmake_minimum_required(VERSION 3.5)

# src/binディレクトリ内の.cppファイルを取得
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(GUI STATIC ${SOURCES})

target_include_directories(GUI PUBLIC include)

# GLFWのリンク
target_link_libraries(GUI PUBLIC GL glfw GLEW)

# FreeType
find_package(Freetype REQUIRED)
target_include_directories(GUI PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(GUI PRIVATE ${FREETYPE_LIBRARIES})
