// C#のプロパティを再現する
// https://qiita.com/CdecPGL/items/ef9c93458b0841001977

#include <utility>

// 可変長のis_same
template <class T, class... Ts>
using are_same = std::conjunction<std::is_same<T, Ts>...>;
// struct are_same : std::conjunction<std::is_same<T, Ts>...> {};

// メンバ関数ポインタからクラスの型を取得するエイリアステンプレート
template <class C, typename TReturn, typename... TArgs>
auto getClassTypeImpl(TReturn (C::*member_function)(TArgs...)) -> C;
template <auto member_function_pointer>
using getClassType = decltype(getClassTypeImpl(member_function_pointer));

// メンバ関数から引数の型を取得する
template <class C, typename TReturn, typename TArg>
auto getArgTypeImpl(TReturn (C::*member_function)(TArg)) -> TArg;
template <auto member_function_pointer>
using getArgType = decltype(getArgTypeImpl(member_function_pointer));

// メンバ関数から戻り値の型を取得する
template <class C, typename R, class... A>
auto getReturnTypeImpl(R (C::*member_function)(A...)) -> R;
template <auto member_function_pointer>
using getReturnType = decltype(getReturnTypeImpl(member_function_pointer));

// 読み取り専用プロパティ
template <auto getter>
class PropertyGet {
    using C = getClassType<getter>;
    using R = getReturnType<getter>;

  public:
    PropertyGet(C *p)
        : p_(p) {}

    // プロパティ型への変換
    operator R() const { return get(); }

    /*()を用いたアクセス*/
    R operator()() const { return get(); }

    // 算術演算子
    R operator+(const R &x) const { return get() + x; }
    friend R operator+(const R &x, const PropertyGet &self) { return x + self.get(); }

    R operator*(const R &x) const { return get() * x; }
    friend R operator*(const R &x, const PropertyGet &self) { return x * self.get(); }

  protected:
    R get() const { return (p_->*getter)(); }

  private:
    C *const p_;
};

// 書き込み専用プロパティ
template <auto setter>
class PropertySetOne {
  protected:
    using C = getClassType<setter>;
    using A = getArgType<setter>;

    C *const p_;

  public:
    PropertySetOne() = delete;
    PropertySetOne(C *p)
        : p_(p) {}

    void set(const A &v) { (p_->*setter)(v); }

    // 代入
    PropertySetOne &operator=(const A &x) {
        set(x);
        return *this;
    }
};

// 書き込み専用プロパティ
template <auto setter, auto... overload_setters>
class PropertySet : public PropertySetOne<setter>, public PropertySetOne<overload_setters>... {
    using C = typename PropertySetOne<setter>::C;
    // using C_same = typename PropertySet<setter2>::C;
    static_assert(are_same<typename PropertySetOne<setter>::C, typename PropertySetOne<overload_setters>::C...>::value, "同じクラスのメンバ関数でないといけません");
    // // using A = getArgType<setter>;

  public:
    PropertySet(C *p)
        : PropertySetOne<setter>(p)
        , PropertySetOne<overload_setters>(p)... {}

    using PropertySetOne<setter>::operator=;
    using PropertySetOne<overload_setters>::operator=...;
    // using PropertySet<setter2>::operator=;
};

// 読み書き可能プロパティ
template <auto getter, auto setter>
class PropertyGetSet : public PropertyGet<getter>,
                       public PropertySetOne<setter> {
    using C = getClassType<getter>;
    using A = getArgType<setter>;
    using R = getReturnType<getter>;
    static_assert(std::is_same_v<C, getClassType<setter>>,
                  "The class of Getter and Setter must be same.");
    // using TGetProperty = PropertyGet<getter>;
    // using TSetProperty = PropertySet<setter>;

  public:
    PropertyGetSet(C *p)
        : PropertyGet<getter>(p)
        , PropertySetOne<setter>(p) {}
    // 代入
    PropertyGetSet &operator=(const A &x) {
        this->set(x);
        return *this;
    }

    // // 前置インクリメント
    // PropertyGetSet &operator++() {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(++buf);
    //     return *this;
    // }

    // // 後置インクリメント
    // TProperty operator++(int) {
    //     auto buf = TGetProperty::get();
    //     auto ret = buf++;
    //     TSetProperty::set(buf);
    //     return ret;
    // }

    // // 前置デクリメント
    // PropertyGetSet &operator--() {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(--buf);
    //     return *this;
    // }

    // // 後置デクリメント
    // TProperty operator--(int) {
    //     auto buf = TGetProperty::get();
    //     auto ret = buf--;
    //     TSetProperty::set(buf);
    //     return ret;
    // }

    // // 複合代入演算子（四則演算）
    // template <typename T>
    // PropertyGetSet &operator+=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf += other);
    //     return *this;
    // }

    // template <typename T>
    // PropertyGetSet &operator-=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf -= other);
    //     return *this;
    // }

    // template <typename T>
    // PropertyGetSet &operator*=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf *= other);
    //     return *this;
    // }

    // template <typename T>
    // PropertyGetSet &operator/=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf /= other);
    //     return *this;
    // }

    // template <typename T>
    // PropertyGetSet &operator%=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf %= other);
    //     return *this;
    // }

    // // 複合代入演算子（ビット演算）
    // template <typename T>
    // PropertyGetSet &operator|=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf |= other);
    //     return *this;
    // }

    // template <typename T>
    // PropertyGetSet &operator&=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf &= other);
    //     return *this;
    // }

    // template <typename T>
    // PropertyGetSet &operator^=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf ^= other);
    //     return *this;
    // }

    // // 複合代入演算子（シフト演算）
    // template <typename T>
    // PropertyGetSet &operator>>=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf >>= other);
    //     return *this;
    // }

    // template <typename T>
    // PropertyGetSet &operator<<=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf <<= other);
    //     return *this;
    // }
};
