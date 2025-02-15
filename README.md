# ビルド方法

## Windows

開発者用コマンドプロンプトにて、
```bat
cmake -B build
:: ビルド
cmake --build build --config Release --target main
:: 実行
.\build\Release\main
```


## Linux

GLEW, glm, FreeType, Luaが必要。
```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_CXX_COMPILER:FILEPATH=clang++ -B ./build -G "Ninja Multi-Config"
cmake --build build --target main  # ビルド
./build/Debug/main  # 実行
```
configureの部分（一行目）はvscodeのcmake拡張などでもok

`--target`後にはmainの他にも、`src/bin/`直下にある.cppファイルの拡張子を抜いた名前が入る。

## clangd

CMakeのコンフィグを実行すると、成果物ディレクトリ直下に`compile_commands.json`が自動生成される。しかし、clangdは（何も設定しない限り）プロジェクトルートディレクトリに置いてある`compile_commands.json`しか見つけてくれない。

したがって、自動生成したjsonを手動なり何なりでプロジェクトルートにコピー/リンクする必要がある。

VSCodeを使っている場合は`"cmake.copyCompileCommands": "${workspaceRoot}/compile_commands.json"`と設定するのがおすすめ。


# 動作確認
clang16
libstdc++

~~警告：libc++では動作しません。~~ 普通に動く

警告 TODO: g++だとテンプレート関連のエラー？でコンパイルできない


WaylandとGLFWの相性が悪く、GTKから警告が出たり、複数回の`glfwInit()`が実行時エラーになったりする。
その際は`XDG_SESSION_TYPE=x11 run luatest`などとして、強制的にX11を使わせればよい。


# アプリ一覧
- `main` いろいろテスト。新機能追加時の動作確認などに使う。
- `blank` 必要最小限度の動作確認用。マウス（もしくは矢印キー）で視点移動、WASDで移動。シフト/スペースで上下移動。Z/Xを長押しすると移動速度が変わる。Qで終了。
- `lines` 2D表示（射影カメラ）および動的に変化するメッシュのテスト。
- `potential_well` 1次元の物理シミュレーション。
- `bricks` ブロック崩し。
