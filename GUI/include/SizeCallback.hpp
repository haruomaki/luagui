#pragma once

#include <Property.hpp>
#include <graphical_base.hpp>

class Window;

enum CallbackKind {
    Size,
    Key,
};

// ユーザはこのクラス（を継承したクラス）を必ずwindow.make_child経由でインスタンス化する
class WindowObject {
    friend class Window;
    Window *window_; // コンストラクト後は変更されない

    // コンストラクタのかわりにグローバル変数を利用してwindow_を初期化するための内部関数
    static Window *get_window_static();
    static void set_window_static(Window *window);

  public:
    // このクラスのコンストラクタは呼ぶ直前にsetWindowStatic()を実行しておかなければならない
    WindowObject()
        : window_(get_window_static()) {
        if (this->window_ == nullptr) {
            std::runtime_error("WindowObjectに関連付けるウィンドウの取得に失敗");
        }
    }
    virtual ~WindowObject() = default;

    // window_を読み取るゲッターおよびプロパティ
    Window &get_window() {
        return *this->window_;
    }
    PropertyGet<&WindowObject::get_window> window{this};
};

class SizeCallback : public WindowObject {
  public:
    std::function<void(int width, int height)> size_callback;

    // SizeCallback() = default;
    // ~SizeCallback() override = default; // virtualにするのが重要
    // SizeCallback(const SizeCallback &) = default;
    // SizeCallback &operator=(const SizeCallback &) = delete;
    // SizeCallback(SizeCallback &&) = default;
    // SizeCallback &operator=(SizeCallback &&) = delete;
};
