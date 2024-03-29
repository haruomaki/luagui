cmake_minimum_required(VERSION 3.5)

# srcディレクトリ内の.cppファイルを取得
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(GUI STATIC ${SOURCES})

target_include_directories(GUI PUBLIC include)

# GLFWのリンク
target_link_libraries(GUI PUBLIC GL glfw GLEW)

# libpng
find_package(PNG REQUIRED)
target_include_directories(GUI SYSTEM PUBLIC ${PNG_INCLUDE_DIRS}) # SYSTEMを指定して警告対象から除外
target_link_libraries(GUI PRIVATE ${PNG_LIBRARIES})

# FreeType
find_package(Freetype REQUIRED)
target_include_directories(GUI SYSTEM PUBLIC ${FREETYPE_INCLUDE_DIRS}) # SYSTEMを指定して警告対象から除外
target_link_libraries(GUI PRIVATE ${FREETYPE_LIBRARIES})

# Lua
find_package(Lua REQUIRED)
target_include_directories(GUI SYSTEM PUBLIC ${LUA_INCLUDE_DIRS})
target_link_libraries(GUI PRIVATE ${LUA_LIBRARIES})
