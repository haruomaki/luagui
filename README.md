# ビルド方法
GLFW, GLEW, glmが必要。
```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_CXX_COMPILER:FILEPATH=clang++ -B ./build -G "Ninja Multi-Config"
cmake --build build --target main  # ビルド
./build/Debug/main  # 実行
```
configureの部分（一行目）はvscodeのcmake拡張などでもok

`--target`後にはmainの他にも、`src/bin/`直下にある.cppファイルの拡張子を抜いた名前が入る。

# 動作確認
clang16
libstdc++

~~警告：libc++では動作しません。~~ 普通に動く

警告 TODO: g++だとテンプレート関連のエラー？でコンパイルできない


# アプリ一覧
- `main` いろいろテスト。新機能追加時の動作確認などに使う。
- `blank` 必要最小限度の動作確認用。
- `lines` 2D表示（射影カメラ）および動的に変化するメッシュのテスト。
- `potential_well` 1次元の物理シミュレーション。
- `bricks` ブロック崩し。
