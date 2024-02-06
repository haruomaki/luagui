#pragma once

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

    bool erase_function(FunctionId function_id) {
        auto pos = this->functions_.find(function_id);
        if (pos == functions_.end()) {
            warn("キーが見つかりませんでした: ", function_id);
            return false;
        }
        this->functions_.erase(pos);
        return true;
    }

    const std::function<Func> &at(FunctionId id) {
        return this->functions_.at(id);
    }

    // 範囲ベースforで回せるように
    auto begin() const { return this->functions_.begin(); }
    auto end() const { return this->functions_.end(); }
};
