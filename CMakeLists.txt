cmake_minimum_required(VERSION 3.5)
project(cppgui CXX)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
)

# GLFWのリンク
# target_link_libraries(${PROJECT_NAME} PRIVATE GL glfw)

# コンパイルオプション
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20) # -std=c++20 を指定
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF) # GNU拡張を無効化

set(CMAKE_CXX_FLAGS "-Weverything -Wno-c++98-compat -Wno-padded -fPIC -msse4.1 -fcolor-diagnostics")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -pg -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=x86-64 -mtune=generic")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=x86-64 -mtune=generic")
