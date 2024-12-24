#pragma once

#include "property.hpp"
#include <stdexcept> // libc++では不要
#include <string>    // libc++では不要

class GUI;

// ユーザはこのクラス（をpublic継承したクラス）を必ずgui.append_resource()経由でインスタンス化する
class Resource {
    friend class GUI;
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
    GUI &get_gui() {
        return *this->gui_;
    }
    PropertyGet<&Resource::get_gui> gui{this};

    // 名前を読み取り/更新するプロパティ
    const std::string &get_name() {
        return this->name_;
    }
    void set_name(const std::string &name) {
        this->name_ = name;
    }
    PropertyGetSet<&Resource::get_name, &Resource::set_name> name{this};
};
