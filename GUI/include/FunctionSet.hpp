#pragma once

#include <functional>
#include <map>

#include "logger.hpp"

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

    // ループ中に要素の追加や削除をしても問題ないようなforeach文
    template <typename Proc>
        requires std::is_invocable_v<Proc, Func>
    void safe_foreach(Proc &&proc) {
        for (auto it = this->functions_.begin(); it != this->functions_.end(); it++) {
            const auto &function = it->second;
            proc(function);
        }
    }
};
