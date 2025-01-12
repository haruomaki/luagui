---@meta coroutines

---coroutine.yield()と同じ
---@param ...any コルーチン呼び出し元に返す値
---@return any ... 呼び出し元からresumeで指定された値
function Yield(...)
    return coroutine.yield(...)
end

---指定の秒数待機する
---@param seconds number 待機する秒数
---@param action? fun(t:number) 経過時間を引数に取る、毎フレーム実行する関数
function Wait(seconds, action)
    local start = get_time()
    if action == nil then
        while get_time() < start + seconds do
            coroutine.yield()
        end
    else
        local elapsedTime = 0
        while elapsedTime < seconds do
            action(elapsedTime) -- 経過時間を引数として渡す
            coroutine.yield()
            elapsedTime = get_time() - start
        end
    end
end

---指定の関数がtrueを返す限り待ち続ける
---@param condition fun(): boolean
function WaitWhile(condition)
    while condition() do
        coroutine.yield()
    end
end

---指定の関数がfalseを返す限り待ち続ける
---@param condition fun(): boolean
function WaitUntil(condition)
    while not condition() do
        coroutine.yield()
    end
end

--- 指定の時間間隔で永久に実行する
---@param action fun() 実行する関数
---@param interval number 実行間隔（秒）
function Interval(action, interval)
    while true do
        local start = get_time()
        action()
        while get_time() - start < interval do
            coroutine.yield()
        end
    end
end

---ただひたすら無限ループを実行する
---@param action? fun(t:number) 経過時間を引数に取る、毎フレーム実行する関数
function Forever(action)
    if action == nil then
        while true do
            coroutine.yield()
        end
    else
        local start = get_time()
        local elapsedTime = 0
        while true do
            action(elapsedTime) -- 経過時間を引数として渡す
            coroutine.yield()
            elapsedTime = get_time() - start
        end
    end
end

---指定の時間が経過するまで、x秒おきに関数を実行する
---@param action fun() 実行する関数
---@param interval number 実行間隔（秒）
---@param duration number 総実行時間（秒）
function RepeatUntil(action, interval, duration)
    local startTime = get_time()
    while get_time() - startTime < duration do
        local iterationStart = get_time()
        action()
        while get_time() - iterationStart < interval do
            coroutine.yield()
        end
    end
end

-------------
-- Fun系関数
-------------

--- 指定の時間間隔で実行する関数を生成する
---@param action fun() 実行する関数
---@param interval number 実行間隔（秒）
function IntervalFun(action, interval)
    return function()
        Interval(action, interval)
    end
end

---無限ループを実行する関数を生成する
---@param action? fun(t:number) 経過時間を引数に取る、毎フレーム実行する関数
function ForeverFun(action)
    return function()
        Forever(action)
    end
end
