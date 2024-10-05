#include <Texture.hpp>

#include <png.h>

// 画像 https://nn-hokuson.hatenablog.com/entry/2017/02/24/171230
GLuint loadTexture(const string &filename) {
    // テクスチャIDの生成
    GLuint tex_id;
    glGenTextures(1, &tex_id);

    // ファイルの読み込み
    std::ifstream fstr(filename, std::ios::binary);
    const size_t file_size = static_cast<size_t>(fstr.seekg(0, std::ifstream::end).tellg());
    fstr.seekg(0, std::ifstream::beg);
    char *texture_buffer = new char[file_size];
    fstr.read(texture_buffer, file_size);

    // テクスチャをGPUに転送
    const int width = 256, height = 256;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_buffer);

    // テクスチャの設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // テクスチャのアンバインド
    delete[] texture_buffer;
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex_id;
}

// PNG形式の画像ファイルからテクスチャを作成
// 出典：https://proken.mydns.jp/index.php?plugin=attach&refer=pg_koneta%2FPNGInOpenGL&openfile=PNGInOpenGL.html
GLuint create_texture_from_png_file(const char *filename) {
    // PNGファイルを開く
    std::unique_ptr<FILE, int (*)(FILE *)> fp(fopen(filename, "rb"), fclose);
    if (!fp) {
        warn("fopenに失敗");
        return 0;
    }

    // PNGファイルを読み込むための構造体を作成
    png_structp p_png = png_create_read_struct(
        PNG_LIBPNG_VER_STRING,
        nullptr, nullptr, nullptr);
    png_infop p_info = png_create_info_struct(p_png);

    // 初期化
    png_init_io(p_png, fp.get());

    // 画像情報を読み込み
    // 画像の幅、高さ、ビット深度、色の表現方法、インターレースの情報を取得する
    int depth, color_type, interlace_type;
    unsigned int width, height;
    png_read_info(p_png, p_info);
    png_get_IHDR(p_png, p_info,
                 &width, &height,
                 &depth, &color_type,
                 &interlace_type, nullptr, nullptr);

    // RGBとRGBAのみに対応
    if (color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGBA) {
        warn("カラー形式はRGBとRGBAのみ対応です");
        return 0;
    }

    // インターレースは非対応
    if (interlace_type != PNG_INTERLACE_NONE) {
        warn("インターレース画像は非対応です");
        return 0;
    }

    // 1行のデータサイズと画像の高さから必要なメモリ量を計算して、メモリ確保
    size_t row_size = png_get_rowbytes(p_png, p_info);
    size_t img_size = row_size * height;
    std::vector<unsigned char> data(img_size);

    // ピクセルの読み込み
    for (size_t i = 0; i < height; i++) {
        png_read_row(p_png, &data[i * row_size], nullptr);
    }

    png_read_end(p_png, p_info);

    // OpenGLテクスチャの作成
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // テクスチャを選択
    glTexImage2D(                          // テクスチャにPNGファイルから読み込んだピクセルを書き込む
        GL_TEXTURE_2D, 0, GL_RGBA,
        (GLsizei)width, (GLsizei)height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // 拡大フィルタリング方法
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 縮小フィルタリング方法
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);      // 横方向にループ
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);      // 縦方向にループ

    // 後片付け
    png_destroy_info_struct(p_png, &p_info);
    png_destroy_read_struct(&p_png, nullptr, nullptr);

    return texture;
}
