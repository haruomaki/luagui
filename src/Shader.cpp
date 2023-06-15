#include <Shader.hpp>
#include <guibase.hpp>

DEFINE_RUNTIME_ERROR(GLShaderCreationException);

// 個々のシェーダをコンパイルする
void compileShader(GLuint shader_id, const string &source_code) {
    // コンパイル
    const char *source_code_char = source_code.c_str();
    glShaderSource(shader_id, 1, &source_code_char, nullptr);
    glCompileShader(shader_id);

    // コンパイル結果のチェック
    GLint compiled;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        // 失敗していたらエラーログの例外を投げる
        GLsizei buf_size; // ログメッセージの長さ
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &buf_size);

        auto info_log = make_unique<GLchar[]>(buf_size); // ログの文字列を格納する領域
        glGetShaderInfoLog(shader_id, buf_size, nullptr, info_log.get());

        throw GLShaderCreationException(info_log.get());
    }
}

// シェーダ https://gist.github.com/tasonco/73a7291769278320c76928b1bd963cba
GLuint createShader() {
    // バーテックスシェーダのコンパイル
    auto vsh_id = glCreateShader(GL_VERTEX_SHADER);
    auto vsh_string = loadString("shader/shader.vsh");
    compileShader(vsh_id, vsh_string);

    // フラグメントシェーダのコンパイル
    auto fsh_id = glCreateShader(GL_FRAGMENT_SHADER);
    auto fsh_string = loadString("shader/shader.fsh");
    compileShader(fsh_id, fsh_string);

    // プログラムオブジェクトの作成
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vsh_id);
    glAttachShader(program_id, fsh_id);

    // リンク
    glLinkProgram(program_id);

    glUseProgram(program_id);

    return program_id;
}
