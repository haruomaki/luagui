// C#のプロパティを再現する
// https://qiita.com/CdecPGL/items/ef9c93458b0841001977

#include <functional>
#include <meta.hpp>

// 読み取り専用プロパティ
template <auto getter>
class PropertyGet {
    using C = getMemberFunctionClass<decltype(getter)>;
    using R = getMemberFunctionRet<decltype(getter)>;

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

    auto operator*(const auto &x) const { return get() * x; }
    friend auto operator*(const auto &x, const PropertyGet &self) { return x * self.get(); }

    auto operator/(const auto &x) const { return get() / x; }
    friend auto operator/(const auto &x, const PropertyGet &self) { return x / self.get(); }

    // template <typename T>
    // auto operator/(const T &x) const { return get() / x; }
    // template <typename T>
    // friend auto operator/(const T &x, const PropertyGet &self) { return x / self.get(); }

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
    Property &operator=(const A &x) { // NOLINT(misc-unconventional-assign-operator) 継承先でusingされることが前提だから
        set(x);
        return static_cast<Property &>(*this);
    }
};

// 書き込み専用プロパティ
template <auto... setters>
class PropertySet : public SetterUnit<PropertySet<setters...>, setters>... {
    // すべてのセッターは同一のクラスCのメンバ関数である必要がある
    static_assert(are_same_v<typename SetterUnit<PropertySet, setters>::C...>, "同じクラスのメンバ関数でないといけません");
    using C = getMemberFunctionClass<getFirstArgType<setters...>>;

  public:
    PropertySet(C *p)
        : SetterUnit<PropertySet, setters>(p)... {}

    using SetterUnit<PropertySet, setters>::operator=...;
};

// template <typename T, typename Property, typename C, typename R, auto setter>
// concept compound_invocable_add = requires(C *c, R r, T t) {
//                                      { setter(c, r + t) } -> std::same_as<Property &>;
//                                  };

template <typename T, typename C, typename R, auto setter, auto right>
concept compound_invocable = std::is_invocable_v<decltype(setter), C, decltype(right(std::declval<R>(), std::declval<T>()))>;

template <typename T, typename C, typename R, auto setter>
concept compound_invocable_add = compound_invocable<T,
                                                    C, R, setter, [](auto x, auto y) {
                                                        return x + y;
                                                    }>;

template <typename T, typename C, typename R, auto... setters>
concept any_compound_invocable_add = (compound_invocable_add<T, C, R, setters> || ...);

static_assert(std::plus<>()(3, 5) == 8, "std::plusです");

// 読み書き可能プロパティ
template <auto getter, auto... setters>
class PropertyGetSet : public PropertyGet<getter>,
                       public SetterUnit<PropertyGetSet<getter, setters...>, setters>... {
    using C = getMemberFunctionClass<decltype(getter)>;
    using R = getMemberFunctionRet<decltype(getter)>;

  public:
    PropertyGetSet(C *p)
        : PropertyGet<getter>(p)
        , SetterUnit<PropertyGetSet, setters>(p)... {}

    // 代入
    // 代入演算子はオーバーロードするために、usingしてこのクラスの直接のメンバとする
    using SetterUnit<PropertyGetSet, setters>::operator=...;

    // なお、ゲッターはusingしない。親クラスのメソッドのままであり、this->get()でアクセスする
    // set()関数もusingしない

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

    // 複合代入演算子（四則演算）
    template <typename T>
        requires any_compound_invocable_add<T, C, R, setters...>
    PropertyGetSet &operator+=(const T &other) {
        return *this = this->get() + other;
    }

    // template <typename T>
    // PropertyGetSet &operator-=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf -= other);
    //     return *this;
    // }

    template <typename T>
    PropertyGetSet &operator*=(const T &other) {
        return *this = this->get() * other;
    }

    template <typename T>
    PropertyGetSet &operator/=(const T &other) {
        return *this = this->get() / other;
    }

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
