cmake_minimum_required(VERSION 3.5)
project(cppgui CXX)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/GUI/Window.cpp
    src/GUI/KeyCallback.cpp
    src/GUI/Update.cpp
    src/GUI/Draw.cpp
    src/GUI/Shader.cpp
)

# GLFWのリンク
target_link_libraries(${PROJECT_NAME} PRIVATE GL glfw GLEW)

# コンパイルオプション
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20) # -std=c++20 を指定
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF) # GNU拡張を無効化

# set(IGNORED_WARNINGS
# -Wno-c++98-compat
# -Wno-padded # 構造体にパディングが挿入されるとき
# -Wno-ctad-maybe-unsupported # 型推論によってテンプレート引数を省略
# )

# CMAKE_CXX_FLAGSにフラグが設定される前にフラグを追加する
# add_compile_options(-Weverything ${IGNORED_WARNINGS})
add_compile_options(-Weverything
    -Wno-c++98-compat
    -Wno-padded # 構造体にパディングが挿入されるとき
    -Wno-ctad-maybe-unsupported # 型推論によってテンプレート引数を省略
)

set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -fPIC -msse4.1 -fcolor-diagnostics")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=x86-64 -mtune=generic")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=x86-64 -mtune=generic")
