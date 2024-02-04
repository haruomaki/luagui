# ビルド方法
```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -B ./build -G "Ninja Multi-Config"
cmake --build build --target main  # ビルド
./build/Debug/main  # 実行
```
configureの部分（一行目）はvscodeのcmake拡張などでもok

`--target`後にはmainの他にも、`src/bin/`直下にある.cppファイルの拡張子を抜いた名前が入る。

# 動作確認
clang16
libstdc++

警告：libc++では動作しません。
