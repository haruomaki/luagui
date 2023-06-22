#include <Property.hpp>
#include <iostream>

class Test {
  private:
    int _value{777};
    int get_value() const { return _value; }
    void set_value(const int &v) {
        std::cout << "セットされた！！！: " << v << std::endl;
        _value = v;
    }

  public:
    // プロパティ型とgetterとなるconstメンバ関数、setterとなるメンバ関数をテンプレート引数に渡す。コンストラクタには自クラスの参照を渡します
    get_set_property<int, &Test::get_value, &Test::set_value> get_set_value{
        *this};
};

int main() {
    Test test;
    // set_only_propertyのように値の設定ができて
    test.get_set_value = 4946;
    // get_only_propertyのように値の参照ができるだけでなく
    std::cout << test.get_set_value << std::endl;
    // 加算代入やシフト代入などもできてしまう
    test.get_set_value += 3;
    std::cout << test.get_set_value << std::endl;
    test.get_set_value <<= 10;
    std::cout << test.get_set_value << std::endl;
}
