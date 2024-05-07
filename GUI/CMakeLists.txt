cmake_minimum_required(VERSION 3.11)

# srcディレクトリ内の.cppファイルを取得
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(GUI STATIC ${SOURCES})

target_include_directories(GUI PUBLIC include)

# *.pcファイルを読み込むツール
find_package(PkgConfig REQUIRED)

# OpenGL & GLEW & GLFW
find_package(OpenGL REQUIRED)
# message("${OPENGL_FOUND} ${OPENGL_INCLUDE_DIRS} ${OPENGL_LIBRARIES} ぐ")
find_package(GLEW REQUIRED)
# message("${GLEW_FOUND} ${GLEW_INCLUDE_DIRS} ${GLEW_LIBRARIES} お")
pkg_check_modules(GLFW REQUIRED glfw3)  # .pcファイルを読み込む
# message("${GLFW_FOUND} ${GLFW_INCLUDE_DIRS} ${GLFW_LIBRARIES} ふ")
target_include_directories(GUI SYSTEM PRIVATE ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})
target_link_libraries(GUI PRIVATE ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES})

# libpng
find_package(PNG REQUIRED)
target_include_directories(GUI SYSTEM PRIVATE ${PNG_INCLUDE_DIRS}) # SYSTEMを指定して警告対象から除外
target_link_libraries(GUI PRIVATE ${PNG_LIBRARIES})

# FreeType
find_package(Freetype REQUIRED)
target_include_directories(GUI SYSTEM PRIVATE ${FREETYPE_INCLUDE_DIRS}) # SYSTEMを指定して警告対象から除外
target_link_libraries(GUI PRIVATE ${FREETYPE_LIBRARIES})

# Box2D
target_include_directories(GUI SYSTEM PUBLIC /usr/include/box2d) # TODO: PRIVATEでもいい
target_link_libraries(GUI PRIVATE box2d)

# Lua
find_package(Lua REQUIRED)
target_include_directories(GUI SYSTEM PRIVATE ${LUA_INCLUDE_DIRS})
target_link_libraries(GUI PRIVATE ${LUA_LIBRARIES})


# 変数のダンプ
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message("${_variableName}=${${_variableName}}")
# endforeach()
