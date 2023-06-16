#include <GUI.hpp>
#include <Shader.hpp>

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
        // 失敗していたらエラーログの例外を投げる https://qiita.com/jasmingirl@github/items/135a58ff27e3c934d15c#シェーダがちゃんとコンパイルできたか確認する
        GLsizei buf_size; // ログメッセージの長さ
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &buf_size);

        auto info_log = make_unique<GLchar[]>(buf_size); // ログの文字列を格納する領域
        glGetShaderInfoLog(shader_id, buf_size, nullptr, info_log.get());

        throw GLShaderCreationException(info_log.get());
    }

    return shader_id;
}

ProgramObject::ProgramObject(std::initializer_list<GLuint> shader_ids) {
    // プログラムオブジェクトの作成
    GLuint program_id = glCreateProgram();
    for (auto &&shader_id : shader_ids) {
        glAttachShader(program_id, shader_id);
    }

    // リンク
    glLinkProgram(program_id);

    program_id_ = program_id;
}

GLuint ProgramObject::getPrgramId() const {
    return program_id_;
}

void ProgramObject::use() const {
    glUseProgram(program_id_);
}

template <>
GLint ProgramObject::getLocation<Attribute>(const string &name) {
    return glGetAttribLocation(program_id_, name.c_str());
}

template <>
GLint ProgramObject::getLocation<Uniform>(const string &name) {
    return glGetUniformLocation(program_id_, name.c_str());
}
