cmake_minimum_required(VERSION 3.5)
project(cppgui CXX)

# コンパイルオプション
# すべてのサブディレクトリにも適用される
add_compile_options(
    # -Wall
    # -Wextra
    -Weverything
    -Wno-c++98-compat
    -Wno-c++98-compat-pedantic # 列挙体のtrailingコンマ
    -Wno-padded # 構造体にパディングが挿入されるとき
    -Wno-ctad-maybe-unsupported # 型推論によってテンプレート引数を省略
    -Wno-shadow-field # メンバ変数と同名の引数
    -Wno-shadow-field-in-constructor # メンバ変数と同名のコンストラクタ引数
    -Wno-unused-macros # ヘッダで定義するマクロ
    
    # 書きやすさに関する警告
    -Wno-implicit-float-conversion # float値リテラルのfを省略
    -Wno-sign-conversion # unsignedありなしの暗黙キャストを許可
    -Wno-old-style-cast # Cスタイルのキャスト記法を許可
    -Wno-shadow # 変数などのシャドーイングを許可

    # TODO: 重要さが未知数で、オンにしたほうがいいかもしれない警告
    # -Wno-documentation-unknown-command # doxygenのコマンドチェック無効化。FreeTypeなど外部ライブラリのヘッダファイルで出てくるためOFFにするしかない
    # -Wno-documentation # ドキュメントコメントでない箇所でのdoxygen表記に警告？これもfreetype.hで出てくる
    -Wno-weak-vtables # TODO: 知識不足
    -Wno-header-hygiene # ヘッダファイルでusing namespaceしてはいけないという警告。これはONでもよさそう
    -Wno-non-virtual-dtor # virtualでないデストラクタへの警告なのだが、「ポリモーフィズムを想定した親クラス」以外にはvirtualは不要であるにも関わらず警告を出してしまうためOFFにする
)
set(CMAKE_CXX_FLAGS "-std=c++23 -fPIC -msse4.1 -fcolor-diagnostics")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=x86-64 -mtune=generic")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=x86-64 -mtune=generic")

add_subdirectory(GUI)

# src/binディレクトリ内の.cppファイルを取得
file(GLOB_RECURSE SOURCES "src/bin/*.cpp")

# .cppファイルごとにadd_executable()を適用
foreach(SOURCE_FILE ${SOURCES})
    # ファイル名から拡張子とディレクトリを削除してターゲット名を作成
    get_filename_component(TARGET_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} PRIVATE GUI)
endforeach()
