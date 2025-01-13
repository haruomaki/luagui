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

# Launchbox（マルチメディア読み書き＆アセット埋め込み）
add_subdirectory(lunchbox)
target_link_libraries(GUI PUBLIC lunchbox)

# Box2D & box2cpp Worldから直接扱えるようにPUBLIC
if (MSVC)
    add_compile_options(/wd4244 /wd4018) # 暗黙の型変換＆比較の警告を抑制
endif()
add_subdirectory(box2d)
target_include_directories(GUI SYSTEM PUBLIC ../external/box2cpp/include)
target_link_libraries(GUI PUBLIC box2d)

# Bullet Physics
find_package(Bullet REQUIRED)
target_link_libraries(GUI PUBLIC ${BULLET_COLLISION_LIBRARY} ${BULLET_DYNAMICS_LIBRARY} ${BULLET_MATH_LIBRARY})
target_include_directories(GUI SYSTEM PUBLIC ${BULLET_INCLUDE_DIR})


# # コンパイル済みヘッダの設定 NOTE: 逆に遅い？
# target_precompile_headers(GUI PUBLIC include/GUI/master.hpp)


# 変数のダンプ
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message("${_variableName}=${${_variableName}}")
# endforeach()
