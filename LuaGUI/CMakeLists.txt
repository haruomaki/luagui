cmake_minimum_required(VERSION 3.11)

# # ヘッダーファイルのディレクトリを指定（不要？）
# include_directories(include ../GUI/include)

# srcディレクトリ内の.cppファイルを取得
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(LuaGUI SHARED ${SOURCES}) # 動的ライブラリを作成

# libGUIにリンク
target_link_libraries(LuaGUI PRIVATE GUI)

target_include_directories(LuaGUI PUBLIC include)

# solのインクルードパス
target_include_directories(LuaGUI SYSTEM PUBLIC ../external)

# コンパイル済みヘッダの設定
target_precompile_headers(LuaGUI PUBLIC ../external/sol/sol.hpp)
