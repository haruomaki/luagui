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

// 個々のセッターを表す
// セッターのオーバーロードの個数だけ、このクラスを継承する
template <typename SelfType, auto setter>
class SetterUnit {
  protected:
    using C = getClassType<setter>;
    using A = getArgType<setter>;

    C *const p_;

  public:
    SetterUnit() = delete;
    SetterUnit(C *p)
        : p_(p) {}

    void set(const A &v) { (p_->*setter)(v); }

    // PropertySet -> A -> PropertySet
    // where Aはセッターの引数型
    SelfType &operator=(const A &x) {
        set(x);
        return static_cast<SelfType &>(*this);
    }
};

// 書き込み専用プロパティ
template <auto setter, auto... overload_setters>
class PropertySet : public SetterUnit<PropertySet<setter, overload_setters...>, setter>, public SetterUnit<PropertySet<setter, overload_setters...>, overload_setters>... {
    using C = typename SetterUnit<PropertySet, setter>::C;
    static_assert(are_same<C, typename SetterUnit<PropertySet, overload_setters>::C...>::value, "同じクラスのメンバ関数でないといけません");

  public:
    PropertySet(C *p)
        : SetterUnit<PropertySet, setter>(p)
        , SetterUnit<PropertySet, overload_setters>(p)... {}

    using SetterUnit<PropertySet, setter>::operator=;
    using SetterUnit<PropertySet, overload_setters>::operator=...;
};

// 読み書き可能プロパティ
template <auto getter, auto setter>
class PropertyGetSet : public PropertyGet<getter>,
                       public SetterUnit<PropertyGetSet<getter, setter>, setter> {
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
        , SetterUnit<PropertyGetSet, setter>(p) {}
    // 代入
    using SetterUnit<PropertyGetSet, setter>::operator=;

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
