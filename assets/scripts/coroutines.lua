-- coroutines.lua

---coroutine.yield()と同じ
---@param ...any コルーチン呼び出し元に返す値
---@return any ... 呼び出し元からresumeで指定された値
function Yield(...)
    return coroutine.yield(...)
end

---指定の秒数待機する
---@param seconds integer 待機する秒数
function Wait(seconds)
    local start = get_time()
    while get_time() <= start + seconds do
        coroutine.yield()
    end
end

--- 毎フレーム実行しつつ指定の秒数待機する
---@param seconds number 待機する秒数
---@param action fun(t:number) 経過時間を引数に取る毎フレーム実行する関数
function WaitWith(seconds, action)
    local start = get_time()
    local elapsedTime = 0
    while elapsedTime <= seconds do
        action(elapsedTime) -- 経過時間を引数として渡す
        coroutine.yield()
        elapsedTime = get_time() - start
    end
end

function WaitUntil(condition)
    while not condition() do
        coroutine.yield()
    end
end
