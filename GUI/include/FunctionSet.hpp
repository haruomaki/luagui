#include <functional>
#include <map>

#include <base.hpp>

using FunctionId = int;

template <typename Func>
class FunctionSet {
    std::map<FunctionId, std::function<Func>> functions_ = {};
    FunctionId function_id_counter_ = 0;

  public:
    FunctionId set_function(std::function<Func> &&func) {
        this->functions_[this->function_id_counter_] = std::move(func);
        return function_id_counter_++;
    }

    void erase_function(FunctionId function_id) {
        // for (const auto &[id, f] : this->functions_) {
        //     debug(id);
        // }
        print("検索〜♪");
        auto pos = this->functions_.find(function_id);
        print("検索おわり");
        if (pos == functions_.end()) {
            print("キーが見つかりませんでした: ", function_id);
        } else {
            print("消すよ！: ", function_id);
            this->functions_.erase(pos);
            print("消したよ！");
        }
    }

    // 範囲ベースforで回せるように
    auto begin() const { return this->functions_.begin(); }
    auto end() const { return this->functions_.end(); }
};
