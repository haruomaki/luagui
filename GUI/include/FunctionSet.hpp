#include <functional>
#include <map>

using FunctionId = int;

template <typename Func>
class FunctionSet {
  public:
    std::map<FunctionId, std::function<Func>> functions_;
    FunctionId function_id_counter_ = 0;
    FunctionId set_function(std::function<Func> &&func) {
        this->functions_[this->function_id_counter_] = std::move(func);
        return function_id_counter_++;
    }
};
