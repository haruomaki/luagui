※すでに書いたソースコードから推測で仕組みを書いているので間違いがあるかも

# 描画の流れ

頂点バッファオブジェクト(VBO)には頂点の座標と色を保持する。
あらかじめVBOをバインドしておくと、`glVertexAttribPointer`を実行したときにVBOに入っているデータを参照してくれる。

VAOをglGenVertexArrays(1, &vao.array_); // VAOの生成
- `glBindVertexArray(array)`でVAOをバインド
glVertexAttribPointer
