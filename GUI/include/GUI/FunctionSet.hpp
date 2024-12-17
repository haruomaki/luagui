#pragma once

#include <SumiGL/logger.hpp>
#include <functional>
#include <map>

using FunctionId = int;

// FIXME: BufferedSetで完全に置き換える
template <typename Func>
class FunctionSet {
    std::map<FunctionId, std::function<Func>> functions_ = {};
    FunctionId function_id_counter_ = 0;
    bool locked_ = false;
    std::vector<std::pair<FunctionId, std::function<Func>>> set_queue_{};
    std::vector<FunctionId> erase_queue_;

    void set_function(FunctionId id, std::function<Func> &&func) {
        this->functions_[id] = std::move(func);
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

  public:
    FunctionId request_set_function(std::function<Func> &&func) {
        auto id = this->function_id_counter_++;
        if (this->locked_) {
            this->set_queue_.emplace_back(id, std::move(func));
        } else {
            this->set_function(id, std::move(func));
        }
        return id;
    }

    void request_erase_function(FunctionId function_id) {
        if (this->locked_) {
            this->erase_queue_.emplace_back(function_id);
        } else {
            this->erase_function(function_id);
        }
    }

    const std::function<Func> &at(FunctionId id) {
        return this->functions_.at(id);
    }

    // ループ中に要素の追加や削除をしても問題ないようなforeach文
    template <typename Proc>
        requires std::is_invocable_v<Proc, Func>
    void safe_foreach(Proc &&proc) {
        this->locked_ = true;

        // foreachを実行
        for (auto it = this->functions_.begin(); it != this->functions_.end(); it++) {
            const auto &function = it->second;
            proc(function);
        }

        // 追加 / 削除キューをリフレッシュ
        for (auto [id, func] : this->set_queue_) {
            this->set_function(id, std::move(func));
        }
        for (auto id : this->erase_queue_) {
            this->erase_function(id);
        }
        this->set_queue_.clear();
        this->erase_queue_.clear();

        this->locked_ = false;
    }
};
