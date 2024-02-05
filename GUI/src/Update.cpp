#include <Update.hpp>

Update::Update(World &world)
    : world(world) {
    update_ = [this] { print("らむらむ"); this->update(); };
    world.updates_.insert(&update_);
    print("登録したよ: ", &update_);
}
Update::~Update() {
    print("Updateのデストラクタ");
    debug(world.updates_.size());
    debug(world.updates_);
    // for (auto *update : world.updates_) {
    //     (*update)();
    //     print("んにゃ");
    //     // debug(update);
    //     // world.updates_.erase(update);
    //     // print("けしたぜ");
    // }
    // world.updates_.clear();
    world.updates_.erase(&update_);      // FIXME: 現状Windowが消滅したあとにUpdateが解放される。
    print("Updateのデストラクタおわり"); // デバッグプリントもなぜかうまく動かないけど、上の文を消したらエラーは消えるのでやはりここがメモリエラーの原因のはず
}
