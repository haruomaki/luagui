#pragma once

#include "Resource.hpp"
#include <SumiGL/Context.hpp>
#include <SumiGL/buffered_container.hpp>

struct Material;
class World;
class Window;

class GUI : public GL::Context {
    bool looping_ = false;
    long epoch_ = 0;

    std::set<std::unique_ptr<Resource>> resources_;
    std::vector<std::unique_ptr<World>> worlds_;

  public:
    BufferedSet<Window *> windows;
    BufferedSet<std::function<void()> *> resource_updates;
    Window *current_window = nullptr; // 現在操作対象としているウィンドウ

    GUI();
    ~GUI();
    GUI(GUI &) = delete;
    GUI &operator=(GUI &) = delete;
    GUI(GUI &&) = delete;
    GUI &operator=(GUI &&) = delete;

    // メインループに制御を移す。
    template <typename F = void (*)()>
    void mainloop(F &&custom_routine = [] {}) {
        if (looping_) {
            throw std::runtime_error("すでにメインループが始まっています");
        }
        looping_ = true;

        // 描画のループ
        while (!windows.empty()) {
            epoch_++;
            trace("[mainloop] メインループ：", epoch_);

            // 受け取ったイベント（キーボードやマウス入力）を処理する
            // キー押下の瞬間などを捉えるために、ユーザ処理よりも前に置く
            glfwPollEvents();

            trace("[mainloop] カスタムルーチン開始：", epoch_);
            custom_routine(); // 削除されたウィンドウへのアクセスを避けるため、ウィンドウ処理よりも前に置く
            trace("[mainloop] ウィンドウ更新開始：", epoch_);

            // リソースの更新処理
            trace("[update] 《resource》->world");
            this->resource_updates.foreach ([](const auto *update) {
                (*update)();
            });

            default_routine1();

            default_routine2();

            // フラッシュ TODO: 場所はここでいい？
            resource_updates.flush();
        }

        looping_ = false;
    }

    // 経過フレーム数を取得する
    [[nodiscard]] long epoch() const { return epoch_; }

    World &create_world();
    void default_routine1();
    void default_routine2();

    // -----------------
    // リソース関係
    // -----------------

    template <typename T, typename... Args>
        requires std::constructible_from<T, Args...> && // ArgsはTのコンストラクタの引数
                 std::convertible_to<T *, Resource *>   // TはResourceの派生クラス
    T &append_resource(Args &&...args) {
        // WindowObjectのコンストラクタを呼ぶ直前には必ずset_gui_staticを呼び、直後nullptrにリセット
        Resource::set_gui_static(this);
        // argsを引数として使って、ヒープ上にT型のオブジェクトを作成
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...); // NOTE: &&やforwardは必要かよく分からない
        Resource::set_gui_static(nullptr);

        auto [it, inserted] = this->resources_.insert(std::move(ptr));
        if (!inserted) {
            throw std::runtime_error("make_childに失敗");
        }
        auto ptr2 = dynamic_cast<T *>(it->get());
        return *ptr2;
    }

    template <typename T = Resource>
        requires std::convertible_to<T *, Resource *> // TはResourceの派生クラス
    T *find_resource(const std::string &name) {
        // NOTE: 虱潰しに検索するので簡潔だが非効率
        // 名前とリソースの対応を表すmapを新たに作りたいが、リソースの追加・削除によるバグが怖い
        for (const auto &rc : this->resources_) {
            if (rc->name_ == name) return dynamic_cast<T *>(rc.get());
        }
        return nullptr;
    }
};
