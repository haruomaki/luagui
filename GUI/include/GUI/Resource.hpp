#pragma once

#include "property.hpp"
#include <stdexcept> // libc++では不要
#include <string>    // libc++では不要

class Window;

// ユーザはこのクラス（をpublic継承したクラス）を必ずwindow.append_resource()経由でインスタンス化する
class Resource {
    friend class Window;
    Window *window_; // コンストラクト後は変更されない

    std::string name_;

    // コンストラクタのかわりにグローバル変数を利用してwindow_を初期化するための内部関数
    static Window *get_window_static();
    static void set_window_static(Window *window);

  public:
    // このクラスのコンストラクタは呼ぶ直前にsetWindowStatic()を実行しておかなければならない
    Resource()
        : window_(get_window_static()) {
        if (this->window_ == nullptr) {
            throw std::runtime_error("WindowObjectに関連付けるウィンドウの取得に失敗");
        }
    }
    virtual ~Resource() = default;

    // window_を読み取るゲッターおよびプロパティ
    Window &get_window() {
        return *this->window_;
    }
    PropertyGet<&Resource::get_window> window{this};

    // 名前を読み取り/更新するプロパティ
    const std::string &get_name() {
        return this->name_;
    }
    void set_name(const std::string &name) {
        this->name_ = name;
    }
    PropertyGetSet<&Resource::get_name, &Resource::set_name> name{this};
};
