// C#のプロパティを再現する
// https://qiita.com/CdecPGL/items/ef9c93458b0841001977

#pragma once

#include "meta.hpp"
#include <functional>

// 読み取り専用プロパティ
template <auto getter>
class PropertyGet {
    using C = get_member_function_class<decltype(getter)>;
    using R = get_member_function_ret<decltype(getter)>;

  public:
    PropertyGet(C *p)
        : p_(p) {}

    // プロパティ型への変換
    operator R() const { return get(); }

    /*()を用いたアクセス*/
    R operator()() const { return get(); }

    // 配列添え字演算子
    auto &operator[](const auto &index) const { return get()[index]; }

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
    using C = get_member_function_class<decltype(setter)>;
    using A = get_member_function_arg<decltype(setter)>;

    C *const p_;

    SetterUnit(C *p)
        : p_(p) {}

    void set(const A &v) { (p_->*setter)(v); }

    // SetterUnit(を継承した型) -> A -> Property(そのもの)
    // where Aはセッターの引数型
    Property &operator=(const A &x) { // NOLINT(misc-unconventional-assign-operator) 継承先でusingされることが前提だから、SetterUnit&ではなくProperty&（PropertySetやPropertyGetSetが入る）を返す
        set(x);
        return static_cast<Property &>(*this);
    }
};

// 書き込み専用プロパティ
template <auto... setters>
class PropertySet : public SetterUnit<PropertySet<setters...>, setters>... {
    // すべてのセッターは同一のクラスCのメンバ関数である必要がある
    static_assert(are_same_v<typename SetterUnit<PropertySet, setters>::C...>, "同じクラスのメンバ関数でないといけません");
    using C = get_member_function_class<get_first_arg_type<setters...>>;

  public:
    PropertySet(C *p)
        : SetterUnit<PropertySet, setters>(p)... {}

    using SetterUnit<PropertySet, setters>::set...;
    using SetterUnit<PropertySet, setters>::operator=...;
};

// template <typename op, typename T, typename S>
// struct OpRet {
//     using type = decltype(op<>()(std::declval<T>() + std::declval<S>()))
// };
//

// setter(getter() ⊕ t) がコンパイル可能かどうかチェックする
// getter()はクラスCのメンバ関数で、R型を返す
// setter()も同じくCのメンバ関数。戻り値型は任意(普通はvoid)
// tはT型の変数
// ⊕は一例。演算子Opはstd::plus<>などを指定する
template <typename T, template <class = void> typename Op, typename C, typename R, auto setter>
concept CompoundInvocableUnit = std::is_invocable_v<decltype(setter), C, get_operator_result<Op, R, T>>;

// 可変n個のsetterに対して、setter(getter() ⊕ t) が一つでもコンパイル可能かどうかチェックする
// ①getterからCとRを抽出する
// ②パックを展開して各setterをCompoundInvacableOneに渡し、判定のorを取る
template <typename T, template <class = void> typename Op, auto getter, auto... setters>
concept CompoundInvocable = (CompoundInvocableUnit<T, Op, get_member_function_class<decltype(getter)>, get_member_function_ret<decltype(getter)>, setters> || ...);

// 読み書き可能プロパティ
template <auto getter, auto... setters>
class PropertyGetSet : public PropertyGet<getter>,
                       public SetterUnit<PropertyGetSet<getter, setters...>, setters>... {
    using C = get_member_function_class<decltype(getter)>;
    using R = get_member_function_ret<decltype(getter)>;

  public:
    PropertyGetSet(C *p)
        : PropertyGet<getter>(p)
        , SetterUnit<PropertyGetSet, setters>(p)... {}

    // 代入
    // set()と代入演算子はオーバーロードするために、usingしてこのクラスの直接のメンバとする
    using SetterUnit<PropertyGetSet, setters>::set...;
    using SetterUnit<PropertyGetSet, setters>::operator=...;

    // なお、ゲッターはusingしない。親クラスのメソッドのままであり、this->get()でアクセスする

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
    PropertyGetSet &operator+=(const CompoundInvocable<std::plus, getter, setters...> auto &other) {
        return *this = *this + other;
        // NOTE: 以下と等価
        // this->set(this->get() + other); ←これがコンパイル可能かどうかをCompoundInvocableで調べている
        // return *this;
    }

    // template <typename T>
    // PropertyGetSet &operator-=(const T &other) {
    //     auto buf = TGetProperty::get();
    //     TSetProperty::set(buf -= other);
    //     return *this;
    // }

    PropertyGetSet &operator*=(const CompoundInvocable<std::multiplies, getter, setters...> auto &other) {
        return *this = *this * other;
    }

    PropertyGetSet &operator/=(const CompoundInvocable<std::divides, getter, setters...> auto &other) {
        return *this = *this / other;
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
