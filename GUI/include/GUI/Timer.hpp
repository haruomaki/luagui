#pragma once

#include "FunctionSet.hpp"

#include <chrono>

/// 複数のタスクを一定間隔で実行するためのタイマー管理機能を提供します。
class Timer {
    struct TaskInfo {
        /// タスクの実行間隔
        const std::chrono::duration<double> interval;
        /// タスクが最後に実行された時間
        std::chrono::high_resolution_clock::time_point last_time;
    };

    FunctionSet<void()> tasks_{};               ///< 登録されたタスクを管理するコンテナ
    std::map<FunctionId, TaskInfo> task_infos_; ///< タスクのIDとその情報を紐付けたマップ
    std::vector<FunctionId> ids_to_erase_;      ///< 削除予約されたタスクのIDリスト

    // Worldクラスがタイマーを所有し毎フレーム更新する
    friend class World;

    /// 登録されたタスクを管理・実行します。
    /// 削除予約されたタスクを安全に削除した後、各タスクの経過時間を確認し、必要に応じて実行します。
    void step() {
        // 削除予約されたタスクを削除
        // mapのイテレーション最中に要素を削除してはいけない
        for (auto &&id : this->ids_to_erase_) {
            this->task_infos_.erase(id);
            this->tasks_.request_erase_function(id);
        }
        this->ids_to_erase_.clear();

        // 各タスクの実行タイミングを確認し、必要なら実行
        for (auto &[id, task_info] : this->task_infos_) {
            auto now = std::chrono::high_resolution_clock::now();
            auto delta = now - task_info.last_time;
            if (delta > task_info.interval) {
                task_info.last_time = now;
                this->tasks_.at(id)();
            }
        }
    }

  public:
    /// 新しいタスクを登録します。
    /// @param interval タスクを実行する間隔（秒）
    /// @param callback 実行するコールバック関数
    /// @return 登録されたタスクに対応する一意のID
    FunctionId task(double interval, std::function<void()> &&callback) {
        auto id = this->tasks_.request_set_function(std::forward<std::function<void()>>(callback));
        this->task_infos_.emplace(std::piecewise_construct,
                                  std::forward_as_tuple(id),
                                  std::forward_as_tuple(std::chrono::duration<double>(interval), std::chrono::high_resolution_clock::time_point()));
        return id;
    }

    /// 登録されたタスクを削除します。
    /// 実行中のタスク削除による問題を防ぐため、削除予約を行います。
    /// @param id 削除したいタスクのID
    void erase(FunctionId id) {
        this->ids_to_erase_.push_back(id);
    }
};
