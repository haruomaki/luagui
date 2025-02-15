cmake_minimum_required(VERSION 3.12)

# vcpkgがあれば利用
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    message("vcpkgを利用します。${CMAKE_TOOLCHAIN_FILE}")

    # プラットフォームごとの設定
    if(WIN32)
        # Windowsの設定
        set(VCPKG_TARGET_TRIPLET "x64-windows-static")
    elseif(APPLE)
        # macOSの設定
        set(VCPKG_TARGET_TRIPLET "x64-osx-static")
    elseif(UNIX)
        # Linuxの設定
        set(VCPKG_TARGET_TRIPLET "x64-linux-static")
    endif()

    message("ターゲットトリプレット: ${VCPKG_TARGET_TRIPLET}")
endif()
  #  if(VCPKG_TARGET_TRIPLET MATCHES "windows")
   #     set(VCPKG_TARGET_TRIPLET "${VCPKG_TARGET_TRIPLET}-static")
    #endif()

# compile_commands.jsonを生成
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(cppgui CXX)

# CMakeLists.txt内でコンパイラを指定する
# set(CMAKE_CXX_COMPILER "clang++17")
# もしくは`cmake -DCMAKE_CXX_COMPILER=***`とする
# vscodeのkit設定: ~/.local/share/CMakeTools/cmake-tools-kits.jsonを編集 https://qiita.com/kilo/items/05e48d22ceff55049b29

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

# コンパイルオプション（-Weverythingを使いたいためclangのときだけ）
# 適用したいプロジェクトに対して、target_compile_options(*** PRIVATE ${WARNINGS})とする
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set (WARNINGS
        # -Wall
        # -Wextra
        -Weverything
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic # 列挙体のtrailingコンマ
        -Wno-padded # 構造体にパディングが挿入されるとき
        -Wno-ctad-maybe-unsupported # 型推論によってテンプレート引数を省略
        -Wno-shadow-field # メンバ変数と同名の引数
        -Wno-shadow-field-in-constructor # メンバ変数と同名のコンストラクタ引数
        -Wno-switch-default # defaultラベルの無いswitch文を許容
        -Wno-unused-macros # ヘッダで定義するマクロ
        -Wno-unsafe-buffer-usage # 変数を添字にしてstd::arrayにアクセスするのを許可
        
        # 書きやすさに関する警告
        -Wno-implicit-float-conversion # float値リテラルのfを省略
        -Wno-sign-conversion # unsignedありなしの暗黙キャストを許可
        -Wno-old-style-cast # Cスタイルのキャスト記法を許可
        -Wno-shadow # 変数などのシャドーイングを許可
       
        # 重要さが未知数で、オンにしたほうがいいかもしれない警告
        -Wno-weak-vtables # TODO: 知識不足
        -Wno-header-hygiene # ヘッダファイルでusing namespaceしてはいけないという警告。TODO: これはONでもよさそう
       
        # その他Clang固有のオプション
        -ferror-limit=100
    )
else()
    set(WARNINGS "")
endif()

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_STANDARD 20) # -std=c++20
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    # ソースファイルのエンコードをUTF-8に決め打ち
    set(CMAKE_CXX_FLAGS "/Zc:preprocessor /EHsc /utf-8 /MP")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "")

    # WindowsでBulletをデバッグビルドするのに必要？
#    add_compile_options(
 #       $<$<CONFIG:Debug>:/MTd>
  #  )

  add_compile_options(
        $<$<CONFIG:>:/MT>
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:RelWithDebInfo>:/MT>
        $<$<CONFIG:MinSizeRel>:/MT>
        $<$<CONFIG:Release>:/MT>
    )
else()
    set(CMAKE_CXX_FLAGS "-fPIC -msse4.1")
    set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=x86-64 -mtune=generic")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=x86-64 -mtune=generic")
endif()

add_subdirectory(GUI)
add_subdirectory(LuaGUI)

# src/binディレクトリ内の.cppファイルを取得
file(GLOB_RECURSE SOURCES "src/bin/*.cpp")

# .cppファイルごとにadd_executable()を適用
foreach(SOURCE_FILE ${SOURCES})
    # ファイル名から拡張子とディレクトリを削除してターゲット名を作成
    get_filename_component(TARGET_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} PRIVATE GUI LuaGUI)
    target_compile_options(${TARGET_NAME} PRIVATE ${WARNINGS})
endforeach()
