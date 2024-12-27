#pragma once

#include <SumiGL/buffered_container.hpp>
#include <chrono>
#include <functional>

/// 複数のタスクを一定間隔で実行するためのタイマー管理機能を提供します。
class Timer {
    using F = std::function<void()>;
    struct TaskInfo {
        /// 関数の実体
        std::unique_ptr<F> f;
        /// タスクの実行間隔
        std::chrono::duration<double> interval;
        /// タスクが最後に実行された時間
        std::chrono::high_resolution_clock::time_point last_time;
    };

    BufferedMap<F *, TaskInfo> tasks_; ///< 登録されたタスクを管理するコンテナ

  public:
    using FunctionId = F *;

    /// 登録されたタスクを管理・実行します。
    /// 削除予約されたタスクを安全に削除した後、各タスクの経過時間を確認し、必要に応じて実行します。
    void step() {
        // 削除予約されたタスクを削除
        tasks_.flush();

        // 各タスクの実行タイミングを確認し、必要なら実行
        tasks_.foreach ([&](TaskInfo &task_info) {
            auto now = std::chrono::high_resolution_clock::now();
            auto delta = now - task_info.last_time;
            if (delta > task_info.interval) {
                task_info.last_time = now;
                (*task_info.f)();
            }
        });
    }

    /// 新しいタスクを登録します。
    /// @param interval タスクを実行する間隔（秒）
    /// @param callback 実行するコールバック関数
    /// @return 登録されたタスクに対応する一意のID
    FunctionId task(double interval, F &&callback) {
        auto f = std::make_unique<F>(std::move(callback));
        auto *p = f.get();
        TaskInfo info = {std::move(f), std::chrono::duration<double>(interval), std::chrono::high_resolution_clock::time_point()};
        tasks_.request_set(p, std::move(info));
        return p;
    }

    /// 登録されたタスクを削除します。
    /// 実行中のタスク削除による問題を防ぐため、削除予約を行います。
    /// @param id 削除したいタスクのID
    void erase(FunctionId id) {
        tasks_.request_erase(id);
    }
};
