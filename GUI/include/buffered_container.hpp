#pragma once

#include <unordered_map>
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

    template <typename Proc>
        requires std::is_invocable_v<Proc, T, T>
    void foreach_flush_combination(Proc &&proc) {
        this->locked_ = true;
        set_all();
        erase_all();

        auto proc_row = [&](auto it1) -> auto {
            for (auto it2 = std::next(it1); it2 != this->elems_.end();) {
                // もしも1つ目の値が削除要請されていたらただちに中断
                auto search1 = this->outs_.find(*it1);
                if (search1 != this->outs_.end()) {
                    auto next_it1 = this->elems_.erase(it1);
                    this->outs_.erase(search1); // 削除リスト自体からも削除
                    return next_it1;
                }

                // もしも2つ目の値が削除要請されていたらそこだけスキップ
                auto search2 = this->outs_.find(*it2);
                if (search2 != this->outs_.end()) {
                    it2 = this->elems_.erase(it2);
                    this->outs_.erase(search2); // 削除リスト自体からも削除
                    continue;
                }

                // どちらも削除しなくてよいのならprocを実行
                proc(*it1, *it2++);
            }
            return ++it1;
        };

        // 削除リストを反映させながらforeachを二重に実行
        for (auto it1 = this->elems_.begin(); it1 != this->elems_.end();) {
            it1 = proc_row(it1);
        }

        set_all();
        erase_all();
        this->locked_ = false;
    }
};

template <typename Key, typename Value>
class BufferedMultimap {
  private:
    std::unordered_multimap<Key, std::unique_ptr<Value>> elements_;
    bool locked_ = false;
    std::vector<std::pair<Key, std::unique_ptr<Value>>> insertions_;
    std::vector<std::pair<Key, Value *>> deletions_;

    void apply_insertions() {
        for (auto &[key, value] : insertions_) {
            elements_.emplace(key, std::move(value));
        }
        insertions_.clear();
    }

    void apply_deletions() {
        for (const auto &[key, value_ptr] : deletions_) {
            auto range = elements_.equal_range(key);
            for (auto it = range.first; it != range.second; ++it) {
                if (it->second.get() == value_ptr) {
                    elements_.erase(it);
                    break;
                }
            }
        }
        deletions_.clear();
    }

  public:
    void request_insert(const Key &key, std::unique_ptr<Value> value) {
        if (locked_) {
            insertions_.emplace_back(key, std::move(value));
        } else {
            elements_.emplace(key, std::move(value));
        }
    }

    void request_delete(const Key &key, Value *value) {
        if (locked_) {
            deletions_.emplace_back(key, value);
        } else {
            auto range = elements_.equal_range(key);
            for (auto it = range.first; it != range.second; ++it) {
                if (it->second.get() == value) {
                    elements_.erase(it);
                    break;
                }
            }
        }
    }

    template <Fn<void(Key, Value &)> Func>
    void foreach_flush(Func &&func) {
        locked_ = true;
        apply_insertions();
        apply_deletions();

        for (auto it = elements_.begin(); it != elements_.end();) {
            auto current = it++;
            func(current->first, *(current->second));
        }

        apply_insertions();
        apply_deletions();
        locked_ = false;
    }

    template <Fn<void(Value &)> Func>
    void foreach_equal(const Key &key, Func &&func) {
        locked_ = true;
        apply_insertions();
        apply_deletions();

        auto range = elements_.equal_range(key);
        for (auto it = range.first; it != range.second;) {
            auto current = it++;
            func(*(current->second));
        }

        apply_insertions();
        apply_deletions();
        locked_ = false;
    }

    Value *at(const Key &key) {
        auto it = elements_.find(key);
        if (it == elements_.end()) return nullptr;
        return it->second.get();
    }

    // const Value &at(const Key &key) const {
    //     // const_cast を使用して、非const版の at() を呼び出す
    //     return const_cast<BufferedMultimap *>(this)->at(key);
    // }
};
