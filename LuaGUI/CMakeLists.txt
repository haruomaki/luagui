cmake_minimum_required(VERSION 3.11)

# # ヘッダーファイルのディレクトリを指定（不要？）
# include_directories(include ../GUI/include)

# LuaGUIについて、動的ライブラリか静的ライブラリのどちらをビルドするか。
option(LUAGUI_BUILD_SHARED_LIBS "build LuaGUI as a shared library" OFF)

# srcディレクトリ内の.cppファイルを取得
file(GLOB_RECURSE SOURCES "src/*.cpp")
if (LUAGUI_BUILD_SHARED_LIBS)
    add_library(LuaGUI SHARED ${SOURCES}) # 動的ライブラリを作成
else()
    add_library(LuaGUI STATIC ${SOURCES}) # 静的ライブラリを作成
endif()

# libGUIにリンク
target_link_libraries(LuaGUI PRIVATE GUI)

target_include_directories(LuaGUI PUBLIC include)
target_compile_options(LuaGUI PRIVATE ${WARNINGS})

# Lua
find_package(Lua REQUIRED)
target_include_directories(LuaGUI SYSTEM PRIVATE ${LUA_INCLUDE_DIRS})
target_link_libraries(LuaGUI PUBLIC ${LUA_LIBRARIES}) # 動的リンクでPUBLIC

# solのインクルードパス
target_include_directories(LuaGUI SYSTEM PUBLIC ../external)

# コンパイル済みヘッダの設定
target_precompile_headers(LuaGUI PUBLIC ../external/sol/sol.hpp)
