#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>

/**
 * @class FreeTypeException
 * @brief FreeTypeライブラリに関連するエラーを表す例外クラス。
 *
 * このクラスは、FreeTypeライブラリの操作中に発生するエラーを表すために使用されます。
 * std::runtime_errorを継承しており、エラーメッセージを保持します。
 */
class FreeTypeException : public std::runtime_error {
  public:
    /**
     * @brief FreeTypeExceptionのコンストラクタ。
     * @param message エラーメッセージ。
     */
    explicit FreeTypeException(const std::string &message)
        : std::runtime_error(message) {}
};

/**
 * @class FreeTypeContext
 * @brief FreeTypeライブラリの初期化とクリーンアップを管理します。
 *
 * このクラスは、FreeTypeライブラリが適切に初期化され、クリーンアップされることを保証します。
 * 任意の時点でこのクラスのインスタンスは一つだけ存在できます。複数のインスタンスを作成しようとすると、
 * 実行時エラーが発生します。
 *
 * 使用例:
 * @code
 * try {
 *     FreeTypeContext ftContext;
 *     FT_Face face = ftContext.load_font("path/to/font.ttf");
 *     // faceオブジェクトを使用してフォント操作を行います
 * } catch (const std::runtime_error& e) {
 *     std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * このクラスはムーブセマンティクスをサポートしますが、コピーは許可されていません。
 * これにより、FreeTypeライブラリが複数回初期化されることを防ぎます。
 *
 * @note
 * - FreeTypeライブラリの複数回の初期化を防ぐため、このクラスはコピー不可です。
 * - 所有権の移動を許可するため、このクラスはムーブ可能です。
 */
class FreeTypeContext {
    FT_Library library_ = nullptr; ///< FreeTypeライブラリのインスタンス。
    static int instance_count;     ///< FreeTypeContextインスタンスの数。

  public:
    /**
     * @brief FreeTypeContextを構築し、FreeTypeライブラリを初期化します。
     * @throws FreeTypeException FreeTypeライブラリが既に初期化されているか、初期化に失敗した場合にスローされます。
     */
    FreeTypeContext();

    /**
     * @brief FreeTypeContextを破棄し、FreeTypeライブラリをクリーンアップします。
     */
    ~FreeTypeContext();

    // コピーは禁止
    FreeTypeContext(const FreeTypeContext &) = delete;
    FreeTypeContext &operator=(const FreeTypeContext &) = delete;

    // ムーブは可能
    FreeTypeContext(FreeTypeContext &&other) noexcept;
    FreeTypeContext &operator=(FreeTypeContext &&other) noexcept;

    /**
     * @brief 指定されたファイルパスからフォントを読み込みます。
     * @param font_path フォントファイルのパス。
     * @return FT_Face 読み込まれたフォントフェイス。
     * @throws FreeTypeException フォントの読み込みに失敗した場合にスローされます。
     */
    FT_Face load_font(const std::string &font_path);
};
