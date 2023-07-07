#pragma once

#include <GUI.hpp>

GLuint createShader(GLenum shader_type, const string &source_code);

enum StorageQualifier {
    Attribute,
    Uniform,
};

class ProgramObject {
    GLuint program_id_;

  public:
    ProgramObject() = default; // FIXME: glCreateProgram()をしないコンストラクタはナンセンス
    ProgramObject(std::initializer_list<GLuint> shader_ids);
    [[nodiscard]] GLuint getPrgramId() const;
    void use() const;

    template <StorageQualifier q>
    [[nodiscard]] GLint getLocation(const string &name) const;

    void setAttribute(const string &name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) const;

    void setUniform(const string &name, GLint int_value) const;
    void setUniform(const string &name, const glm::vec3 &vec3_value) const;
    void setUniform(const string &name, const glm::vec4 &vec4_value) const;
    void setUniform(const string &name, const glm::mat4 &mat4_value) const;
};

class VertexArrayObject {
    GLuint array_{};

  public:
    static inline VertexArrayObject gen() {
        VertexArrayObject vao;
        glGenVertexArrays(1, &vao.array_); // VAOの生成
        return vao;
    }

    inline void bind(const function<void()> &proc_in_bind) const {
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
        glBindBuffer(target, xbo.buffer_);
        glBufferData(target, GLsizeiptr(size), data, usage); // バインド中にデータを設定
        glBindBuffer(target, 0);
        return xbo;
    }

    inline void bind(const function<void()> &proc_in_bind) const {
        glBindBuffer(target, buffer_);
        proc_in_bind();
        glBindBuffer(target, 0);
    }
};

using VertexBufferObject = BufferObject<GL_ARRAY_BUFFER>;
