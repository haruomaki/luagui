// C#のプロパティを再現する
// https://qiita.com/CdecPGL/items/ef9c93458b0841001977

#include <utility>

// メンバ関数ポインタからクラスの型を取得するエイリアステンプレート
template <class C, typename TReturn, typename... TArgs>
auto member_function_class_type_impl(TReturn (C::*member_function)(TArgs...) const) -> C;

template <class C, typename TReturn, typename... TArgs>
auto member_function_class_type_impl(TReturn (C::*member_function)(TArgs...)) -> C;

template <auto MemberFunctionPointer>
using member_function_class_type_t = decltype(member_function_class_type_impl(MemberFunctionPointer));

// 読み取り専用プロパティ
template <typename TProperty, auto Getter>
class get_only_property {
    using C = member_function_class_type_t<Getter>;

  public:
    get_only_property(const C &ins)
        : _instance(ins) {}

    // プロパティ型への変換
    operator TProperty() const { return get(); }

    /*()を用いたアクセス*/
    TProperty operator()() const { return get(); }

    // 配列添え字演算子
    template <typename T>
    auto operator[](const T &other) const
        -> decltype(std::declval<TProperty>[other]) {
        return get()[other];
    }

    // 四則演算子
    template <typename T>
    auto operator+(const T &other) const
        -> decltype(std::declval<TProperty>() + other) {
        return get() + other;
    }

    template <typename T>
    auto operator-(const T &other) const
        -> decltype(std::declval<TProperty>() - other) {
        return get() - other;
    }

    template <typename T>
    auto operator*(const T &other) const
        -> decltype(std::declval<TProperty>() * other) {
        return get() * other;
    }

    template <typename T>
    auto operator/(const T &other) const
        -> decltype(std::declval<TProperty>() / other) {
        return get() / other;
    }

    template <typename T>
    auto operator%(const T &other) const
        -> decltype(std::declval<TProperty>() % other) {
        return get() % other;
    }

    // ビット演算子
    template <typename T>
    auto operator|(const T &other) const
        -> decltype(std::declval<TProperty>() | other) {
        return get() | other;
    }

    template <typename T>
    auto operator&(const T &other) const
        -> decltype(std::declval<TProperty>() & other) {
        return get() & other;
    }

    template <typename T>
    auto operator^(const T &other) const
        -> decltype(std::declval<TProperty>() ^ other) {
        return get() ^ other;
    }

    // 論理演算子
    template <typename T>
    bool operator||(const T &other) const {
        return get() || other;
    }

    template <typename T>
    bool operator&&(const T &other) const {
        return get() && other;
    }

    // シフト演算
    template <typename T>
    TProperty operator>>(const T &other) const {
        return get() >> other;
    }

    template <typename T>
    TProperty operator<<(const T &other) const {
        return get() << other;
    }

    // 比較演算子
    template <typename T>
    bool operator==(const T &other) const {
        return get() == other;
    }

    template <typename T>
    bool operator!=(const T &other) const {
        return get() != other;
    }

    template <typename T>
    bool operator>(const T &other) const {
        return get() > other;
    }

    template <typename T>
    bool operator>=(const T &other) const {
        return get() >= other;
    }

    template <typename T>
    bool operator<(const T &other) const {
        return get() < other;
    }

    template <typename T>
    bool operator<=(const T &other) const {
        return get() <= other;
    }

    // 単項演算子
    TProperty operator+() const { return +get(); }
    TProperty operator-() const { return -get(); }
    TProperty operator!() const { return !get(); }
    TProperty operator~() const { return ~get(); }

  protected:
    TProperty get() const { return (_instance.*Getter)(); }

  private:
    const C &_instance;
};

// 書き込み専用プロパティ
template <typename TProperty, auto Setter>
class set_only_property {
    using C = member_function_class_type_t<Setter>;

  public:
    set_only_property(C &ins)
        : _instance(ins) {}

    // 代入
    set_only_property &operator=(const TProperty &v) {
        set(v);
        return *this;
    }

    /*()を用いたアクセス*/
    void operator()(const TProperty &v) { set(v); }

  protected:
    void set(const TProperty &v) { (_instance.*Setter)(v); }

  private:
    C &_instance;
};

// 読み書き可能プロパティ
template <typename TProperty, auto Getter, auto Setter>
class get_set_property : public get_only_property<TProperty, Getter>,
                         public set_only_property<TProperty, Setter> {
    using C = member_function_class_type_t<Getter>;
    static_assert(std::is_same_v<C, member_function_class_type_t<Setter>>,
                  "The class of Getter and Setter must be same.");
    using TGetProperty = get_only_property<TProperty, Getter>;
    using TSetProperty = set_only_property<TProperty, Setter>;

  public:
    get_set_property(C &ins)
        : TGetProperty(ins)
        , TSetProperty(ins) {}
    // 代入
    get_set_property &operator=(const TProperty &v) {
        TSetProperty::operator=(v);
        return *this;
    }

    // 前置インクリメント
    get_set_property &operator++() {
        auto buf = TGetProperty::get();
        TSetProperty::set(++buf);
        return *this;
    }

    // 後置インクリメント
    TProperty operator++(int) {
        auto buf = TGetProperty::get();
        auto ret = buf++;
        TSetProperty::set(buf);
        return ret;
    }

    // 前置デクリメント
    get_set_property &operator--() {
        auto buf = TGetProperty::get();
        TSetProperty::set(--buf);
        return *this;
    }

    // 後置デクリメント
    TProperty operator--(int) {
        auto buf = TGetProperty::get();
        auto ret = buf--;
        TSetProperty::set(buf);
        return ret;
    }

    // 複合代入演算子（四則演算）
    template <typename T>
    get_set_property &operator+=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf += other);
        return *this;
    }

    template <typename T>
    get_set_property &operator-=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf -= other);
        return *this;
    }

    template <typename T>
    get_set_property &operator*=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf *= other);
        return *this;
    }

    template <typename T>
    get_set_property &operator/=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf /= other);
        return *this;
    }

    template <typename T>
    get_set_property &operator%=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf %= other);
        return *this;
    }

    // 複合代入演算子（ビット演算）
    template <typename T>
    get_set_property &operator|=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf |= other);
        return *this;
    }

    template <typename T>
    get_set_property &operator&=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf &= other);
        return *this;
    }

    template <typename T>
    get_set_property &operator^=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf ^= other);
        return *this;
    }

    // 複合代入演算子（シフト演算）
    template <typename T>
    get_set_property &operator>>=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf >>= other);
        return *this;
    }

    template <typename T>
    get_set_property &operator<<=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf <<= other);
        return *this;
    }
};
