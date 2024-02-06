#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <thread>

using TaskId = unsigned long;

class Timer {
    struct TaskInfo {
        const std::function<void()> callback;
        const std::chrono::duration<double> interval;
        std::chrono::high_resolution_clock::time_point last_time;
    };

    std::map<TaskId, TaskInfo> tasks_{};
    TaskId id_history_ = 0;
    std::vector<TaskId> ids_to_erase_{};

  public:
    TaskId task(double interval, const std::function<void()> &callback) {
        TaskId id = id_history_++;
        this->tasks_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(id),
            std::forward_as_tuple(callback, std::chrono::duration<double>(interval), std::chrono::high_resolution_clock::time_point()));
        return id;
    }

    void erase(TaskId id) {
        this->ids_to_erase_.push_back(id);
    }

    void step() {
        // 削除予約されたタスクを削除
        // mapのイテレーション最中に要素を削除してはいけない
        for (auto &&id : this->ids_to_erase_) {
            this->tasks_.erase(id);
        }
        this->ids_to_erase_.clear();

        for (auto &[id, task_info] : this->tasks_) {
            auto now = std::chrono::high_resolution_clock::now();
            auto delta = now - task_info.last_time;
            if (delta > task_info.interval) {
                task_info.last_time = now;
                task_info.callback();
            }
        }
    }
};
