cmake_minimum_required(VERSION 3.11)

# # ヘッダーファイルのディレクトリを指定
# include_directories(include ../GUI/include)

# 動的ライブラリを作成
add_library(LuaGUI SHARED src/luagui.cpp)

# # libGUIにリンク
# target_link_libraries(LuaGUI PRIVATE GUI)

target_include_directories(LuaGUI PUBLIC include)
