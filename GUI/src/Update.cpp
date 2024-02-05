#include <Update.hpp>

Update::Update(World &world)
    : world(world) {
    update_ = [this] { this->update(); };
    world.updates_.insert(&update_);
}
Update::~Update() {
    print("Updateのデストラクタ");
    world.updates_.erase(&update_);      // FIXME: 現状Windowが消滅したあとにUpdateが解放される。
    print("Updateのデストラクタおわり"); // デバッグプリントもなぜかうまく動かないけど、上の文を消したらエラーは消えるのでやはりここがメモリエラーの原因のはず
}
