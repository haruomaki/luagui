cmake_minimum_required(VERSION 3.5)
project(cppgui CXX)

# コンパイルオプション
# すべてのサブディレクトリにも適用される
# add_compile_options(
# -Weverything
# -Wno-c++98-compat
# -Wno-padded # 構造体にパディングが挿入されるとき
# -Wno-ctad-maybe-unsupported # 型推論によってテンプレート引数を省略
# )
set(CMAKE_CXX_FLAGS "-std=c++20 -save-temps=obj -fPIC -msse4.1 -fcolor-diagnostics")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=x86-64 -mtune=generic")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=x86-64 -mtune=generic")

add_executable(${PROJECT_NAME}
    src/main.cpp
)

add_subdirectory(GUI)
target_link_libraries(${PROJECT_NAME} PRIVATE GUI)

# src/binディレクトリ内の.cppファイルを取得
file(GLOB_RECURSE SOURCES "src/bin/*.cpp")

# .cppファイルごとにadd_executable()を適用
foreach(SOURCE_FILE ${SOURCES})
    # ファイル名から拡張子とディレクトリを削除してターゲット名を作成
    get_filename_component(TARGET_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} PRIVATE GUI)
endforeach()
