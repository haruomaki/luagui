#include <Update.hpp>

Update::Update(Window &window)
    : window(window) {
    update_ = [this] { this->update(); };
    window.updates_.insert(&update_);
}
Update::~Update() {
    print("Updateのデストラクタ");
    window.updates_.erase(&update_);     // FIXME: 現状Windowが消滅したあとにUpdateが解放される。
    print("Updateのデストラクタおわり"); // デバッグプリントもなぜかうまく動かないけど、上の文を消したらエラーは消えるのでやはりここがメモリエラーの原因のはず
}
