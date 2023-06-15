#include <Shader.hpp>
#include <guibase.hpp>

DEFINE_RUNTIME_ERROR(GLShaderCreationException);

// 個々のシェーダを生成する
GLuint createShader(GLenum shader_type, const string &source_code) {
    GLuint shader_id = glCreateShader(shader_type);

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

    return shader_id;
}

// シェーダ https://gist.github.com/tasonco/73a7291769278320c76928b1bd963cba
GLuint createProgram() {
    // バーテックスシェーダのコンパイル
    auto vsh_string = loadString("shader/shader.vsh");
    auto vsh_id = createShader(GL_VERTEX_SHADER, vsh_string);

    // フラグメントシェーダのコンパイル
    auto fsh_string = loadString("shader/shader.fsh");
    auto fsh_id = createShader(GL_FRAGMENT_SHADER, fsh_string);

    // プログラムオブジェクトの作成
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vsh_id);
    glAttachShader(program_id, fsh_id);

    // リンク
    glLinkProgram(program_id);

    glUseProgram(program_id);

    return program_id;
}
