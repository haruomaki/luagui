#pragma once

#include "property.hpp"
#include <memory>
#include <stdexcept> // libc++では不要
#include <string>    // libc++では不要

class GUI;

// ユーザはこのクラス（をpublic継承したクラス）を必ずgui.append_resource()経由でインスタンス化する
class Resource {
    friend class ResourceManager;
    GUI *gui_; // コンストラクト後は変更されない

    std::string name_;

    // コンストラクタのかわりにグローバル変数を利用してgui_を初期化するための内部関数
    static GUI *get_gui_static();
    static void set_gui_static(GUI *gui);

  public:
    // このクラスのコンストラクタは呼ぶ直前にsetWindowStatic()を実行しておかなければならない
    Resource()
        : gui_(get_gui_static()) {
        if (this->gui_ == nullptr) {
            throw std::runtime_error("WindowObjectに関連付けるGUIの取得に失敗");
        }
    }
    virtual ~Resource() = default;

    // window_を読み取るゲッターおよびプロパティ
    [[nodiscard]] GUI &gui() const { return *gui_; }

    // 名前を読み取り/更新するプロパティ
    // NOTE: 名前で検索できるようにする
    [[nodiscard]] const std::string &get_name() const {
        return this->name_;
    }
    void set_name(const std::string &name) {
        this->name_ = name;
    }
    PropertyGetSet<&Resource::get_name, &Resource::set_name> name{this};
};

template <std::derived_from<Resource> T>
class ResourceHandle;

class ResourceManager {
    GUI &gui_;
    std::unordered_map<Resource *, std::unique_ptr<Resource>> elems_;

  public:
    ResourceManager(GUI &gui)
        : gui_(gui) {}

    template <typename T, typename... Args>
        requires std::constructible_from<T, Args...> && // ArgsはTのコンストラクタの引数
                 std::convertible_to<T *, Resource *>   // TはResourceの派生クラス
    ResourceHandle<T> append(Args &&...args);

    template <typename T = Resource>
        requires std::convertible_to<T *, Resource *> // TはResourceの派生クラス
    T *find(const std::string &name) {
        // NOTE: 虱潰しに検索するので簡潔だが非効率
        // 名前とリソースの対応を表すmapを新たに作りたいが、リソースの追加・削除によるバグが怖い
        for (const auto &[id, rc] : this->elems_) {
            if (rc->get_name() == name) return dynamic_cast<T *>(rc.get());
        }
        return nullptr;
    }

    void clear() { elems_.clear(); }
    bool is_valid(Resource *id) { return elems_.contains(id); }
    Resource *get(Resource *id) {
        if (!is_valid(id)) {
            throw std::runtime_error("リソースが削除済みです。");
        }
        return elems_[id].get();
    }
    void free(Resource *id) { elems_.erase(id); }
};

template <std::derived_from<Resource> T>
class ResourceHandle {
    ResourceManager &manager_;
    Resource *id_ = nullptr;

  public:
    ResourceHandle(ResourceManager &manager, Resource *id)
        : manager_(manager)
        , id_(id) {}

    bool is_valid() { return manager_.is_valid(id_); }
    T &get() { return *dynamic_cast<T *>(manager_.get(id_)); }
    void free() { manager_.free(id_); }
};

template <typename T, typename... Args>
    requires std::constructible_from<T, Args...> && // ArgsはTのコンストラクタの引数
             std::convertible_to<T *, Resource *>   // TはResourceの派生クラス
ResourceHandle<T> ResourceManager::append(Args &&...args) {
    // WindowObjectのコンストラクタを呼ぶ直前には必ずset_gui_staticを呼び、直後nullptrにリセット
    Resource::set_gui_static(&gui_);
    // argsを引数として使って、ヒープ上にT型のオブジェクトを作成
    auto ptr = std::make_unique<T>(std::forward<Args>(args)...); // NOTE: &&やforwardは必要かよく分からない
    Resource::set_gui_static(nullptr);

    auto [it, inserted] = this->elems_.emplace(ptr.get(), std::move(ptr));
    if (!inserted) {
        throw std::runtime_error("リソースのappendに失敗");
    }
    auto ptr2 = dynamic_cast<T *>(it->second.get());
    return ResourceHandle<T>(*this, it->second.get());
}
