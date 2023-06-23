#include <Property.hpp>
#include <iostream>

class Test {
  private:
    int value_{777};
    [[nodiscard]] int getValue() const { return value_; }
    void setValue(const int &v) {
        std::cout << "セットされた！！！: " << v << std::endl;
        value_ = v;
    }

  public:
    // プロパティ型とgetterとなるconstメンバ関数、setterとなるメンバ関数をテンプレート引数に渡す。コンストラクタには自クラスの参照を渡します
    GetSetProperty<int, &Test::getValue, &Test::setValue> get_set_value{*this};
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
