#include <Shader.hpp>
#include <graphical_base.hpp>

DEFINE_RUNTIME_ERROR(GLShaderCreationException);

// 個々のシェーダを生成する
GLuint create_shader(GLenum shader_type, const string &source_code) {
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

        auto info_log = std::make_unique<GLchar[]>(buf_size); // ログの文字列を格納する領域
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

ProgramObject::~ProgramObject() {
    glDeleteProgram(this->program_id_);
}

GLuint ProgramObject::get_program_id() const {
    return program_id_;
}

void ProgramObject::use() const {
    glUseProgram(program_id_);
}

template <>
GLint ProgramObject::get_location<Attribute>(const string &name) const {
    return glGetAttribLocation(program_id_, name.c_str());
}

template <>
GLint ProgramObject::get_location<Uniform>(const string &name) const {
    return glGetUniformLocation(program_id_, name.c_str());
}

void ProgramObject::set_attribute(const string &name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) const {
    GLint location = get_location<Attribute>(name);
    glVertexAttribPointer(location, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(location);
}

void ProgramObject::set_uniform(const string &name, GLint int_value) const {
    glUniform1i(get_location<Uniform>(name), int_value);
}

void ProgramObject::set_uniform(const string &name, const glm::vec3 &vec3_value) const {
    glUniform3f(get_location<Uniform>(name), vec3_value.x, vec3_value.y, vec3_value.z);
}

void ProgramObject::set_uniform(const string &name, const glm::vec4 &vec4_value) const {
    glUniform4f(get_location<Uniform>(name), vec4_value.x, vec4_value.y, vec4_value.z, vec4_value.w);
}

void ProgramObject::set_uniform(const string &name, const glm::mat4 &mat4_value) const {
    glUniformMatrix4fv(get_location<Uniform>(name), 1, GL_FALSE, glm::value_ptr(mat4_value));
}
