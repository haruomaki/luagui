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
target_compile_options(GUI PRIVATE ${WARNINGS})

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
target_link_libraries(GUI PUBLIC ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES}) # 動的リンクでPUBLIC

# libpng
find_package(PNG REQUIRED)
target_include_directories(GUI SYSTEM PRIVATE ${PNG_INCLUDE_DIRS}) # SYSTEMを指定して警告対象から除外
target_link_libraries(GUI PRIVATE ${PNG_LIBRARIES})
# message("libpng: ${PNG_FOUND} ${PNG_INCLUDE_DIRS} ${PNG_LIBRARIES}")

# FreeType
find_package(Freetype REQUIRED)
target_include_directories(GUI SYSTEM PRIVATE ${FREETYPE_INCLUDE_DIRS}) # SYSTEMを指定して警告対象から除外
target_link_libraries(GUI PRIVATE ${FREETYPE_LIBRARIES})

# Box2D & box2cpp Worldから直接扱えるようにPUBLIC
add_subdirectory(box2d)
target_include_directories(GUI SYSTEM PUBLIC ../external/box2cpp/include)
target_link_libraries(GUI PUBLIC box2d)


# コンパイル済みヘッダの設定
target_precompile_headers(GUI PUBLIC ./include/master.hpp)


# 変数のダンプ
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message("${_variableName}=${${_variableName}}")
# endforeach()
