#include <FreeType.hpp>

// グローバル変数の初期化
int FreeTypeContext::instance_count = 0;

FreeTypeContext::FreeTypeContext() {
    if (instance_count >= 1) {
        throw FreeTypeException("FreeTypeContextのインスタンスは1つだけしか存在できません。");
    }
    if (FT_Init_FreeType(&library_) != 0) {
        throw FreeTypeException("FreeTypeライブラリの初期化に失敗しました。");
    }
    ++instance_count;
}

FreeTypeContext::~FreeTypeContext() {
    FT_Done_FreeType(library_);
    --instance_count;
}

FreeTypeContext::FreeTypeContext(FreeTypeContext &&other) noexcept { *this = std::move(other); }
FreeTypeContext &FreeTypeContext::operator=(FreeTypeContext &&other) noexcept {
    if (this != &other) {
        library_ = other.library_;
        other.library_ = nullptr;
    }
    return *this;
}

FT_Face FreeTypeContext::load_font(const std::string &font_path) {
    FT_Face face;
    if (FT_New_Face(library_, font_path.c_str(), 0, &face) != 0) {
        throw FreeTypeException("フォントを読み込めませんでした: " + font_path);
    }
    return face;
}
