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

    // Attribute変数やUniform変数の通し番号を取得する。無効な名前の場合は-1が返る。
    // NOTE: GCCにはクラス定義内で明示的特殊化ができないバグがあるため、定数ifで代用。https://stackoverflow.com/a/49707378/22234700
    template <StorageQualifier q>
    [[nodiscard]] inline GLint get_location(const string &name) const {
        GLint loc;
        if constexpr (q == StorageQualifier::Attribute) {
            loc = glGetAttribLocation(program_id_, name.c_str());
            if (loc == -1) warn("Attribute変数名 \"", name, "\" は無効です");
        } else if constexpr (q == StorageQualifier::Uniform) {
            loc = glGetUniformLocation(program_id_, name.c_str());
            if (loc == -1) warn("Uniform変数名 \"", name, "\" は無効です");
        } else {
            static_assert(false, "StorageQualifierの種類が無効ですよ");
        }
        return loc;
    }

    /// シェーダにアトリビュート変数を渡す関数。
    /// glVertexAttribPointer（頂点属性のデータフォーマットを設定）とglEnableVertexAttribArray（頂点属性配列の有効化）を行う。
    /// @param name アトリビュート変数名
    /// @param size アトリビュート変数の要素数。例えばvec3ならば3。
    /// @param type 変数のデータ型。GL_FLOATなど。
    /// @param normalized データを正規化するかどうか。例えば色属性の場合、 `GL_TRUE` にすると0-255の範囲の値が0.0-1.0に正規化される。
    /// @param stride データブロックのサイズ（バイト単位）。連続したデータの場合は0を指定する。例えば位置属性と色属性が交互に格納されている場合、strideは各頂点のデータサイズになる。
    /// @param pointer 配列の開始位置。通常はバッファオブジェクトのオフセットを指定する。
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
        // NOLINTBEGIN(performance-no-int-to-ptr)
        glVertexAttribPointer(pos1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, nullptr);
        glVertexAttribPointer(pos2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, reinterpret_cast<void *>(sizeof(float) * 4));
        glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, reinterpret_cast<void *>(sizeof(float) * 8));
        glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, reinterpret_cast<void *>(sizeof(float) * 12));
        // NOLINTEND(performance-no-int-to-ptr)
        glVertexAttribDivisor(pos1, 1);
        glVertexAttribDivisor(pos2, 1);
        glVertexAttribDivisor(pos3, 1);
        glVertexAttribDivisor(pos4, 1);
    }
};

// VAOを管理するラッパークラス。自動でglGenVertexArraysとglDeleteVertexArraysが呼ばれる。
class VertexArrayObject {
    GLuint array_ = 0;

  public:
    VertexArrayObject()
        : array_([&] {
            GLuint array;
            glGenVertexArrays(1, &array); // BOの生成
            return array;
        }()) {}
    ~VertexArrayObject() {
        if (array_ != 0) {
            glDeleteVertexArrays(1, &array_);
        }
    }
    VertexArrayObject(const VertexArrayObject &) = delete;
    VertexArrayObject &operator=(const VertexArrayObject &) const = delete;
    VertexArrayObject(VertexArrayObject &&other) noexcept
        : array_(other.array_) {
        other.array_ = 0;
    }
    VertexArrayObject &operator=(VertexArrayObject &&other) noexcept {
        if (this != &other) {
            this->array_ = other.array_;
            other.array_ = 0;
        }
        return *this;
    }

    template <typename F>
    inline void bind(const F &proc_in_bind) const {
        glBindVertexArray(array_); // VAOをバインド
        proc_in_bind();
        glBindVertexArray(0);
    }
};

// VBOなどのバッファオブジェクトを管理するラッパークラス。
// ※デフォルトコンストラクタは何も行わない。
template <GLenum target>
class BufferObject {
    // はじめはゼロ初期化。あくまで仮の初期値であり、使う際は引数ありのコンストラクタを呼ばないと無意味。
    GLuint buffer_ = 0;

  public:
    // デフォルトコンストラクタ。バッファは生成されない。
    BufferObject() = default;

    /// バッファを生成する。glGenBuffer->バインドしてglBufferDataを呼び出す。
    /// glBufferDataの各引数を指定する。
    /// @param size VRAMに転送するデータのサイズ。
    /// @param data VRAMに転送するデータの先頭ポインタ。
    /// @param usage GL_STATIC_DRAWやGL_DYNAMIC_DRAWを指定する。
    BufferObject(size_t size, const void *data, GLenum usage)
        : buffer_([&] {
            GLuint buffer;
            glGenBuffers(1, &buffer); // BOの生成
            return buffer;
        }()) {
        this->bind([&] {
            glBufferData(target, GLsizeiptr(size), data, usage); // バインド中にデータを設定
        });
    }

    ~BufferObject() {
        if (buffer_ != 0) {
            glDeleteBuffers(1, &buffer_);
        }
    }
    BufferObject(const BufferObject &) = delete;
    BufferObject &operator=(const BufferObject &) const = delete;
    BufferObject(BufferObject &&other) noexcept
        : buffer_(other.buffer_) {
        other.buffer_ = 0;
    }
    BufferObject &operator=(BufferObject &&other) noexcept {
        if (this != &other) {
            this->buffer_ = other.buffer_;
            other.buffer_ = 0;
        }
        return *this;
    }

    // バッファ内容を更新する
    inline void subdata(GLintptr offset, size_t size, const void *data) {
        this->bind([&] {
            glBufferSubData(target, offset, GLsizeiptr(size), data);
        });
    }

    template <typename F>
    inline void bind(const F &proc_in_bind) const {
        keep_bind();
        proc_in_bind();
        glBindBuffer(target, 0);
    }

    inline void keep_bind() const {
        if (buffer_ == 0) warn("バッファオブジェクトの参照が切れています。ムーブ済み、もしくは空のバッファオブジェクトを仮生成しているのかもしれません。引数付きのコンストラクタを用いてください。");
        glBindBuffer(target, buffer_);
    }
};

using VertexBufferObject = BufferObject<GL_ARRAY_BUFFER>;
using ElementBufferObject = BufferObject<GL_ELEMENT_ARRAY_BUFFER>;
