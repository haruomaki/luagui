#include <GUI.hpp>
#include <utility>

// メンバ関数ポインタからクラスの型を取得するエイリアステンプレート
template <class C, typename TReturn, typename... TArgs>
auto member_function_class_type_impl(TReturn (C::*member_function)(TArgs...) const) -> C;

template <class C, typename TReturn, typename... TArgs>
auto member_function_class_type_impl(TReturn (C::*member_function)(TArgs...)) -> C;

template <auto MemberFunctionPointer>
using getClassType = decltype(member_function_class_type_impl(MemberFunctionPointer));

// 書き込み専用プロパティ
template <typename TProperty, auto Setter>
class PropertySet {
    using C = getClassType<Setter>;

  public:
    PropertySet(C *ins)
        : instance_(*ins) {}

    // 代入
    PropertySet &operator=(const TProperty &v) {
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

class MyClass {
    int value_;

  public:
    void setValue(int value) {
        value_ = value;
    }
    void setValue(float value) {
        value_ = int(value * 100);
    }
    void show() {
        debug(value_);
    }
    PropertySet<int, static_cast<void (MyClass::*)(int)>(&MyClass::setValue)> value{this};
};

// int main() {
//     MyClass ccc;
//     ccc.show();
//     ccc.value = 334;
//     ccc.show();
// }

template <class C, typename TReturn, typename TArg>
auto getArgTypeImpl(TReturn (C::*member_function)(TArg)) -> TArg;

template <auto member_function_pointer>
using getArgType = decltype(getArgTypeImpl(member_function_pointer));

template <class T, auto f1, auto f2, auto f3>
struct Property {
    T *p;
    Property &operator=(getArgType<f1> arg) {
        (p->*f1)(arg);
        return *this;
    }
    Property &operator=(getArgType<f2> arg) {
        (p->*f2)(arg);
        return *this;
    }
    Property &operator=(getArgType<f3> arg) {
        (p->*f3)(arg);
        return *this;
    }
};

class ValueAndHistory {
    int value_ = 0;
    vector<int> history_ = {};

  public:
    void setValue(int value) {
        value_ = value;
        history_.push_back(value);
    }
    void setValue(double value) {
        setValue(int(value));
    }
    void setValue(initializer_list<int> list) {
        for (auto &&value : list) {
            setValue(value);
        }
    }

    void show() {
        cout << "current value: " << value_ << endl;
        cout << "history: ";
        for (auto &&record : history_) {
            cout << record << " ";
        }
        cout << endl;
    }

    Property<ValueAndHistory, static_cast<void (ValueAndHistory::*)(int)>(&ValueAndHistory::setValue), static_cast<void (ValueAndHistory::*)(double)>(&ValueAndHistory::setValue), static_cast<void (ValueAndHistory::*)(initializer_list<int>)>(&ValueAndHistory::setValue)> value{this};
};

int main() {
    ValueAndHistory t;
    t.value = 7;
    t.value = 5;
    t.value = 3.14;
    t.value = {3, 6, 4};

    t.show();
}
