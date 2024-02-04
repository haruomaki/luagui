#pragma once

#include <Property.hpp>
#include <graphical_base.hpp>

class Window;

// ユーザはこのクラス（を継承したクラス）を必ずwindow.makeChild経由でインスタンス化する
class SizeCallback {
    friend class Window;
    Window *window_; // コンストラクト後は変更されない

    // コンストラクタのかわりにグローバル変数を利用してwindow_を初期化するための内部関数
    static Window *get_window_static();
    static void set_window_static(Window *window);

  public:
    std::function<void(int width, int height)> size_callback;

    // このクラスのコンストラクタは呼ぶ直前にsetWindowStatic()を実行しておかなければならない
    SizeCallback()
        : window_(get_window_static()) {
        if (this->window_ == nullptr) {
            std::runtime_error("SizeCallbackに関連付けるウィンドウの取得に失敗");
        }
    }
    virtual ~SizeCallback() = default; // virtualにするのが重要
    SizeCallback(const SizeCallback &) = default;
    SizeCallback &operator=(const SizeCallback &) = delete;
    SizeCallback(SizeCallback &&) = default;
    SizeCallback &operator=(SizeCallback &&) = delete;

    // window_を読み取るゲッターおよびプロパティ
    Window &get_window() {
        return *this->window_;
    }
    PropertyGet<&SizeCallback::get_window> window{this};
};
