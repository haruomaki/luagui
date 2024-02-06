#pragma once

#include "FunctionSet.hpp"

#include <chrono>

class Timer {
    struct TaskInfo {
        const std::chrono::duration<double> interval;
        std::chrono::high_resolution_clock::time_point last_time;
    };

    FunctionSet<void()> tasks_{};
    std::map<FunctionId, TaskInfo> task_infos_{};
    std::vector<FunctionId> ids_to_erase_{};

  public:
    FunctionId task(double interval, std::function<void()> &&callback) {
        auto id = this->tasks_.set_function(std::forward<std::function<void()>>(callback));
        this->task_infos_.emplace(std::piecewise_construct,
                                  std::forward_as_tuple(id),
                                  std::forward_as_tuple(std::chrono::duration<double>(interval), std::chrono::high_resolution_clock::time_point()));
        return id;
    }

    void erase(FunctionId id) {
        this->ids_to_erase_.push_back(id);
    }

    void step() {
        // 削除予約されたタスクを削除
        // mapのイテレーション最中に要素を削除してはいけない
        for (auto &&id : this->ids_to_erase_) {
            this->task_infos_.erase(id);
            this->tasks_.erase_function(id);
        }
        this->ids_to_erase_.clear();

        for (auto &[id, task_info] : this->task_infos_) {
            // 経過時間が登録されたインターバルより長ければタスクを実行
            auto now = std::chrono::high_resolution_clock::now();
            auto delta = now - task_info.last_time;
            if (delta > task_info.interval) {
                task_info.last_time = now;
                this->tasks_.at(id)();
            }
        }
    }
};
