#pragma once

#include <functional>
#include <map>

#include "logger.hpp"

using FunctionId = int;

template <typename Func>
class FunctionSet {
    std::map<FunctionId, std::function<Func>> functions_ = {};
    FunctionId function_id_counter_ = 0;
    bool locked_ = false;
    std::vector<std::pair<FunctionId, std::function<Func>>> set_queue_{};
    std::vector<FunctionId> erase_queue_{};

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

// #include <queue>
#include <unordered_set>

template <typename T>
class BufferedSet {
    std::unordered_set<T> elems_ = {};
    bool locked_ = false;
    std::unordered_set<T> ins_{};
    std::unordered_set<T> outs_{};

    void set(T &&elem) {
        this->elems_.emplace(std::move(elem));
    }

    bool erase(T &&elem) {
        auto pos = this->elems_.find(elem);
        if (pos == this->elems_.end()) {
            warn("キーが見つかりませんでした: ", elem);
            return false;
        }
        this->elems_.erase(pos);
        return true;
    }

    // 追加リストをマージ
    void set_all() {
        this->elems_.merge(this->ins_);
        if (!this->ins_.empty()) warn("BufferedSetの追加要素に重複がありました");
        this->ins_ = std::unordered_set<T>{};
    }

    // 削除リストに基づいて削除
    void erase_all() {
        for (const auto &out : this->outs_) {
            auto to_erase = this->elems_.find(out);
            if (to_erase != this->elems_.end()) {
                this->elems_.erase(to_erase);
            } else {
                warn("BufferedSetの削除リスト中に無効な要素");
            }
        }
        this->outs_ = std::unordered_set<T>{};
    }

  public:
    void request_set(T &&elem) {
        this->ins_.emplace(std::move(elem));
    }

    void request_erase(T &&elem) {
        this->outs_.emplace(std::move(elem));
    }

    // 追加/削除要求を適用させたのちforeachを回す
    // ループ中に要素の追加や削除をしても問題ないようなforeach文
    template <typename Proc>
        requires std::is_invocable_v<Proc, T>
    void foreach_flush(Proc &&proc) {
        this->locked_ = true;
        set_all();
        erase_all();

        // 削除リストを反映させながらforeachを実行
        for (auto it = this->elems_.begin(); it != this->elems_.end();) {
            // もしも削除リストに値があれば現値を削除
            auto search = this->outs_.find(*it);
            if (search != this->outs_.end()) {
                it = this->elems_.erase(it);
                this->outs_.erase(search); // 削除リスト自体からも削除
                continue;
            }
            proc(*it++);
        }

        set_all();
        erase_all();
        this->locked_ = false;
    }
};
