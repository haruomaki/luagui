#pragma once

#include "Resource.hpp"
#include <SumiGL/Context.hpp>
#include <SumiGL/Shader.hpp>

struct Material;

struct GUI {
    GL::Context ctx;
    bool looping = false;
    long epoch = 0;

    std::set<std::unique_ptr<Resource>> resources_;
    BufferedSet<std::function<void()> *> resource_updates;
    std::optional<GL::ProgramObject> default_shader;
    Material *default_material = nullptr;

    GUI();

    ~GUI() {
        this->resources_.clear(); // resource_updatesが消える前にResourceUpdateのデストラクタを呼ぶ
    }

    // メインループに制御を移す。
    template <typename F = void (*)()>
    void mainloop(F &&custom_routine = [] {}) {
        if (looping) {
            throw std::runtime_error("すでにメインループが始まっています");
        }
        looping = true;

        // 描画のループ
        while (!ctx.windows.empty()) {
            epoch++;
            trace("[mainloop] メインループ：", epoch);

            // 受け取ったイベント（キーボードやマウス入力）を処理する
            // キー押下の瞬間などを捉えるために、ユーザ処理よりも前に置く
            glfwPollEvents();

            trace("[mainloop] カスタムルーチン開始：", epoch);
            custom_routine(); // 削除されたウィンドウへのアクセスを避けるため、ウィンドウ処理よりも前に置く
            trace("[mainloop] ウィンドウ更新開始：", epoch);

            // リソースの更新処理
            trace("[update] 《resource》->world");
            this->resource_updates.foreach ([](const auto *update) {
                (*update)();
            });

            // 登録されている各ウィンドウに対してルーティンを実行
            ctx.windows.foreach ([](const GL::Window *window) {
                window->routine();
            });

            // フラッシュ TODO: 場所はここでいい？
            resource_updates.flush();
        }

        looping = false;
    }

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
