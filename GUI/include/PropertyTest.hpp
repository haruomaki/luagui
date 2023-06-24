// C#のプロパティを再現する
// https://qiita.com/CdecPGL/items/ef9c93458b0841001977

#include <utility>

// 可変長のis_same
template <class T, class... Ts>
using are_same = std::conjunction<std::is_same<T, Ts>...>;
// struct are_same : std::conjunction<std::is_same<T, Ts>...> {};

template <typename T = void, typename... Rest>
struct FirstType {
    using type = T;
};

// 型パラメータパックから先頭要素を取得
template <typename... Args>
using getFirstType = typename FirstType<Args...>::type;

// TODO: 引数が空のときvoidを返す
template <auto x, auto... rest>
struct FirstArgType {
    using type = decltype(x);
};

// 値パラメータパックから先頭要素の型を取得
template <auto... args>
using getFirstArgType = typename FirstArgType<args...>::type;

static_assert(std::is_same_v<getFirstArgType<1, 2, 3>, int>);
static_assert(std::is_same_v<getFirstArgType<true, 'a'>, bool>);
// static_assert(std::is_same_v<getFirstArgType<>, void>);

// メンバ関数ポインタ型から各型名を抽出する、特殊化フィルタ
template <typename T>
struct MemberFunctionPointerTypeFilter;
template <typename C, typename R, typename... Args>
struct MemberFunctionPointerTypeFilter<R (C::*)(Args...)> {
    using ClassType = C;
    using FirstArgType = getFirstType<Args...>;
    using ReturnType = R;
};
// メンバ関数ポインタ型から所属クラスを取得
template <typename Pointer>
using getMemberFunctionClass = typename MemberFunctionPointerTypeFilter<Pointer>::ClassType;
// メンバ関数ポインタ型から（第1引数の）引数型を取得
template <typename Pointer>
using getMemberFunctionArg = typename MemberFunctionPointerTypeFilter<Pointer>::FirstArgType;
// メンバ関数ポインタ型から戻り値型を取得
template <typename Pointer>
using getMemberFunctionRet = typename MemberFunctionPointerTypeFilter<Pointer>::ReturnType;

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

// メンバ関数ポインタ型の可変テンプレート引数を受け取り、先頭の関数が属するクラスを返す
template <typename C, typename R, class A, typename... Rest>
auto getFirstElementClassImpl(R (C::*member_function)(A), Rest... rest) -> C;
template <auto... vars>
using getFirstElementClass = decltype(getFirstElementClassImpl(vars...));

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
template <typename Property, auto setter>
class SetterUnit {
  protected:
    using C = getMemberFunctionClass<decltype(setter)>;
    using A = getMemberFunctionArg<decltype(setter)>;

    C *const p_;

  public:
    SetterUnit() = delete;
    SetterUnit(C *p)
        : p_(p) {}

    void set(const A &v) { (p_->*setter)(v); }

    // PropertySet -> A -> PropertySet
    // where Aはセッターの引数型
    Property &operator=(const A &x) {
        set(x);
        return static_cast<Property &>(*this);
    }
};

// 書き込み専用プロパティ
template <auto... setters>
class PropertySet : public SetterUnit<PropertySet<setters...>, setters>... {
    using C = getMemberFunctionClass<getFirstArgType<setters...>>;
    static_assert(are_same<C, typename SetterUnit<PropertySet, setters>::C...>::value, "同じクラスのメンバ関数でないといけません");

  public:
    PropertySet(C *p)
        : SetterUnit<PropertySet, setters>(p)... {}

    using SetterUnit<PropertySet, setters>::operator=...;
};

// 読み書き可能プロパティ
template <auto getter, auto setter>
class PropertyGetSet : public PropertyGet<getter>,
                       public SetterUnit<PropertyGetSet<getter, setter>, setter> {
    using C = getMemberFunctionClass<decltype(getter)>;
    using A = getMemberFunctionArg<decltype(setter)>;
    using R = getMemberFunctionRet<decltype(getter)>;
    // static_assert(std::is_same_v<C, getClassType<setter>>,
    // "The class of Getter and Setter must be same.");
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
