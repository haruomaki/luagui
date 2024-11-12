cmake_minimum_required(VERSION 3.11)

# GUIについて、動的ライブラリか静的ライブラリのどちらをビルドするか。
option(GUI_BUILD_SHARED_LIBS "build GUI as a shared library" OFF)

# srcディレクトリ内の.cppファイルを取得
file(GLOB_RECURSE SOURCES "src/*.cpp")

if (GUI_BUILD_SHARED_LIBS)
    # 動的リンクのためにSHAREDを指定。このとき、いくつかのtarget_link_librariesをPUBLICにする必要がある。
    add_library(GUI SHARED ${SOURCES})
else()
    add_library(GUI STATIC ${SOURCES}) # 静的ライブラリを作成
endif()

target_include_directories(GUI PUBLIC include)
target_include_directories(GUI PRIVATE include/GUI)
target_compile_options(GUI PRIVATE ${WARNINGS})

# SumiGL（OpenGLのラッパー）
add_subdirectory(sumigl)
target_link_libraries(GUI PUBLIC sumigl)

# GLFW https://www.glfw.org/docs/3.3/build_guide.html#build_link_cmake_source
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE) # waylandサポートを無効化（HiDPI環境でのバグを抑制）
add_subdirectory(glfw)
target_link_libraries(GUI PUBLIC glfw) # これだけでインクルードパスも設定してくれるらしい

# libpng
find_package(PNG REQUIRED)
target_include_directories(GUI SYSTEM PRIVATE ${PNG_INCLUDE_DIRS}) # SYSTEMを指定して警告対象から除外
target_link_libraries(GUI PRIVATE ${PNG_LIBRARIES})
# message("libpng: ${PNG_FOUND} ${PNG_INCLUDE_DIRS} ${PNG_LIBRARIES}")

# FuriType（FreeTypeのラッパー）
add_subdirectory(furitype)
target_link_libraries(GUI PUBLIC furitype)

# Box2D & box2cpp Worldから直接扱えるようにPUBLIC
if (MSVC)
    add_compile_options(/wd4244 /wd4018) # 暗黙の型変換＆比較の警告を抑制
endif()
add_subdirectory(box2d)
target_include_directories(GUI SYSTEM PUBLIC ../external/box2cpp/include)
target_link_libraries(GUI PUBLIC box2d)


# コンパイル済みヘッダの設定
target_precompile_headers(GUI PUBLIC include/GUI/master.hpp)


# 変数のダンプ
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message("${_variableName}=${${_variableName}}")
# endforeach()
