#include <Update.hpp>

Update::Update() {
    auto func = [this] { this->update(); };
    this->func_id_ = this->get_world().updates_.set_function(std::move(func));
}
Update::~Update() {
    print("Updateのデストラクタ");
    this->get_world().updates_.erase_function(this->func_id_);
    print("Updateのデストラクタおわり"); // デバッグプリントもなぜかうまく動かないけど、上の文を消したらエラーは消えるのでやはりここがメモリエラーの原因のはず
}
