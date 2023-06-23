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
class GetOnlyProperty {
    using C = member_function_class_type_t<Getter>;

  public:
    GetOnlyProperty(const C &ins)
        : instance_(ins) {}

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
    TProperty get() const { return (instance_.*Getter)(); }

  private:
    const C &instance_;
};

// 書き込み専用プロパティ
template <typename TProperty, auto Setter>
class SetOnlyProperty {
    using C = member_function_class_type_t<Setter>;

  public:
    SetOnlyProperty(C &ins)
        : instance_(ins) {}

    // 代入
    SetOnlyProperty &operator=(const TProperty &v) {
        set(v);
        return *this;
    }

    /*()を用いたアクセス*/
    void operator()(const TProperty &v) { set(v); }

  protected:
    void set(const TProperty &v) { (instance_.*Setter)(v); }

  private:
    C &instance_;
};

// 読み書き可能プロパティ
template <typename TProperty, auto Getter, auto Setter>
class GetSetProperty : public GetOnlyProperty<TProperty, Getter>,
                       public SetOnlyProperty<TProperty, Setter> {
    using C = member_function_class_type_t<Getter>;
    static_assert(std::is_same_v<C, member_function_class_type_t<Setter>>,
                  "The class of Getter and Setter must be same.");
    using TGetProperty = GetOnlyProperty<TProperty, Getter>;
    using TSetProperty = SetOnlyProperty<TProperty, Setter>;

  public:
    GetSetProperty(C &ins)
        : TGetProperty(ins)
        , TSetProperty(ins) {}
    // 代入
    GetSetProperty &operator=(const TProperty &v) {
        TSetProperty::operator=(v);
        return *this;
    }

    // 前置インクリメント
    GetSetProperty &operator++() {
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
    GetSetProperty &operator--() {
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
    GetSetProperty &operator+=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf += other);
        return *this;
    }

    template <typename T>
    GetSetProperty &operator-=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf -= other);
        return *this;
    }

    template <typename T>
    GetSetProperty &operator*=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf *= other);
        return *this;
    }

    template <typename T>
    GetSetProperty &operator/=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf /= other);
        return *this;
    }

    template <typename T>
    GetSetProperty &operator%=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf %= other);
        return *this;
    }

    // 複合代入演算子（ビット演算）
    template <typename T>
    GetSetProperty &operator|=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf |= other);
        return *this;
    }

    template <typename T>
    GetSetProperty &operator&=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf &= other);
        return *this;
    }

    template <typename T>
    GetSetProperty &operator^=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf ^= other);
        return *this;
    }

    // 複合代入演算子（シフト演算）
    template <typename T>
    GetSetProperty &operator>>=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf >>= other);
        return *this;
    }

    template <typename T>
    GetSetProperty &operator<<=(const T &other) {
        auto buf = TGetProperty::get();
        TSetProperty::set(buf <<= other);
        return *this;
    }
};
