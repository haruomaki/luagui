// C#のプロパティを再現する
// https://qiita.com/CdecPGL/items/ef9c93458b0841001977

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
    auto operator+(const auto &x) const { return get() + x; }
    friend auto operator+(const auto &x, const PropertyGet &self) { return x + self.get(); }

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

    using SetterUnit<PropertySet, setters>::operator=...;
};

template <typename Property, auto getter, auto setter>
class GetterSetterUnit : virtual PropertyGet<getter>, public SetterUnit<Property, setter> {
    // class GetterSetterUnit : SetterUnit<Property, setter> {
  protected:
    using C = typename SetterUnit<Property, setter>::C;
    using A = typename SetterUnit<Property, setter>::A;

  private:
    // 代入演算子は明示的に継承する必要がある
    // 複合代入演算子の定義に使用する
    using SetterUnit<Property, setter>::operator=;

  public:
    GetterSetterUnit() = delete;
    GetterSetterUnit(C *p)
        : PropertyGet<getter>(p)
        , SetterUnit<Property, setter>(p) {}

    // 複合代入演算子
    Property &operator+=(const A &x) { // NOLINT(misc-unconventional-assign-operator) 継承先でusingされることが前提だから
        // return *this = this->get() + x;
        return *this = *this + x;
        // return *this = this->operator+(x);
        // return this->operator=(this->operator+(x));
        // return this->operator=(*this + x);
        // this->set(this->operator+(x));
        // return static_cast<Property &>(*this);
    }
};

// 読み書き可能プロパティ
template <auto getter, auto... setters>
class PropertyGetSet : public virtual PropertyGet<getter>,
                       public GetterSetterUnit<PropertyGetSet<getter, setters...>, getter, setters>... {
    using C = getMemberFunctionClass<decltype(getter)>;

  public:
    PropertyGetSet(C *p)
        : PropertyGet<getter>(p)
        , GetterSetterUnit<PropertyGetSet, getter, setters>(p)... {}

    // 代入
    // set()および代入演算子はオーバーロードするために、usingしてこのクラスの直接のメンバとする
    // using GetterSetterUnit<PropertyGetSet, getter, setters>::set...;
    // using GetterSetterUnit<PropertyGetSet, getter, setters>::operator=...;
    using SetterUnit<PropertyGetSet, setters>::set...;
    using SetterUnit<PropertyGetSet, setters>::operator=...;
    // using GetterSetterUnit<PropertyGetSet, getter, setters>::operator+=...; 複合代入演算子は自動的に継承される。

    // なお、ゲッターはusingしない。親クラスのメソッドのままであり、this->get()でアクセスする
    // using PropertyGet<getter>::get;

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

    // template <typename T>
    // PropertyGetSet &operator+=(const T &other) {
    //     return *this = this->get() + other;
    // }
    // using GetterSetterUnit<PropertyGetSet, getter, setters>::operator+=...;

    // template <typename T>
    // PropertyGetSet &operator-=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf -= other);
    //     return *this;
    // }

    // template <typename T>
    // PropertyGetSet &operator*=(const T &other) {
    //     return *this = this->get() * other;
    // }

    // template <typename T>
    // PropertyGetSet &operator/=(const T &other) {
    //     return *this = this->get() / other;
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
