#pragma once

#include <graphical_base.hpp>

GLuint create_shader(GLenum shader_type, const string &source_code);

enum class StorageQualifier {
    Attribute,
    Uniform,
};

class ProgramObject {
    GLuint program_id_;

  public:
    ProgramObject() = delete; // glCreateProgram()をしないコンストラクタはナンセンス
    ProgramObject(std::initializer_list<GLuint> shader_ids);
    ~ProgramObject();
    ProgramObject(const ProgramObject &) = delete;
    ProgramObject &operator=(const ProgramObject &) const = delete;
    ProgramObject(ProgramObject &&shader) noexcept // TODO: ソースファイルに
        : program_id_(shader.program_id_) {
    }
    ProgramObject &operator=(ProgramObject &&shader) noexcept {
        this->program_id_ = shader.program_id_;
        return *this;
    }

    [[nodiscard]] GLuint get_program_id() const;
    void use() const;

    // 無効な名前の場合は-1が返る
    template <StorageQualifier q>
    [[nodiscard]] inline GLint get_location(const string &name) const;
    template <>
    inline GLint get_location<StorageQualifier::Attribute>(const string &name) const {
        auto loc = glGetAttribLocation(program_id_, name.c_str());
        if (loc == -1) warn("Attribute変数名 \"", name, "\" は無効です");
        return loc;
    }
    template <>
    inline GLint get_location<StorageQualifier::Uniform>(const string &name) const {
        auto loc = glGetUniformLocation(program_id_, name.c_str());
        if (loc == -1) warn("Uniform変数名 \"", name, "\" は無効です");
        return loc;
    }

    void set_attribute(const string &name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) const;
    void set_uniform(const string &name, GLint int_value) const;
    void set_uniform(const string &name, const glm::vec3 &vec3_value) const;
    void set_uniform(const string &name, const glm::vec4 &vec4_value) const;
    void set_uniform(const string &name, const glm::mat4 &mat4_value) const;
    void set_divisor(const string &name, GLuint divisor) const;

    void mat4_attribute(const string &name) const {
        int pos = get_location<StorageQualifier::Attribute>(name);
        int pos1 = pos + 0;
        int pos2 = pos + 1;
        int pos3 = pos + 2;
        int pos4 = pos + 3;
        glEnableVertexAttribArray(pos1);
        glEnableVertexAttribArray(pos2);
        glEnableVertexAttribArray(pos3);
        glEnableVertexAttribArray(pos4);
        glVertexAttribPointer(pos1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void *)(0));
        glVertexAttribPointer(pos2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void *)(sizeof(float) * 4));
        glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void *)(sizeof(float) * 8));
        glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void *)(sizeof(float) * 12));
        glVertexAttribDivisor(pos1, 1);
        glVertexAttribDivisor(pos2, 1);
        glVertexAttribDivisor(pos3, 1);
        glVertexAttribDivisor(pos4, 1);
    }
};

class VertexArrayObject {
    GLuint array_{};

  public:
    static inline VertexArrayObject gen() {
        VertexArrayObject vao;
        glGenVertexArrays(1, &vao.array_); // VAOの生成
        return vao;
    }

    template <typename F>
    inline void bind(const F &proc_in_bind) const {
        glBindVertexArray(array_); // VAOをバインド
        proc_in_bind();
        glBindVertexArray(0);
    }
};

template <GLenum target>
class BufferObject {
    GLuint buffer_{};

  public:
    // glGenBuffers() + glBufferData()
    static inline BufferObject gen(size_t size, const void *data, GLenum usage) {
        BufferObject xbo;
        glGenBuffers(1, &xbo.buffer_); // BOの生成
        xbo.bind([&] {
            glBufferData(target, GLsizeiptr(size), data, usage); // バインド中にデータを設定
        });
        return xbo;
    }

    // バッファ内容を更新する
    inline void subdata(GLintptr offset, size_t size, const void *data) {
        this->bind([&] {
            glBufferSubData(target, offset, GLsizeiptr(size), data);
        });
    }

    template <typename F>
    inline void bind(const F &proc_in_bind) const {
        glBindBuffer(target, buffer_);
        proc_in_bind();
        glBindBuffer(target, 0);
    }

    inline void keep_bind() const {
        glBindBuffer(target, buffer_);
    }
};

using VertexBufferObject = BufferObject<GL_ARRAY_BUFFER>;
using ElementBufferObject = BufferObject<GL_ELEMENT_ARRAY_BUFFER>;
